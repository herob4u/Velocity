#include "vctPCH.h"
#include "ResourceQueues.h"

#include "Resource.h"

#include "Engine/IO/FileMgr.h"

using namespace Vct;

ResourceStreamer::ResourceStreamer(ResourceLoader& resourceLoader)
    : m_resourceLoader(resourceLoader)
{
    m_workThread = std::thread(&ResourceStreamer::Execute, this);
}

void ResourceStreamer::EnqueueResources(const std::vector<Resource*>& resources, OnResourcesLoaded cb, LoadPriority priority)
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex);

    if(priority == LoadPriority::DEFAULT)
    {
        m_queue.emplace_back(resources, cb);
    }
    else if(priority == LoadPriority::HIGH)
    {
        m_queue.emplace_front(resources, cb);
    }
}

void ResourceStreamer::Execute()
{
    FileMgr& fileMgr = FileMgr::Get();

    std::unique_lock<std::mutex> queueLock(m_queueMutex, std::defer_lock);
    while(1)
    {
        if(m_queue.size() > 0)
        {
            if(queueLock.try_lock())
            {
                if(m_queue.size() > 0)
                {
                    // CRITICAL
                    AsyncItem item = m_queue.front();
                    m_queue.pop_front();
                    // END CRITICAL

                    queueLock.unlock();

                    // Process Item
                    for(Resource* res : item.Resources)
                    {
                        // Blocking load
                        // Resource may be destroyed while we wait for loading
                        //if(res)
                        //res->BeginLoad(true);

                        if(res->IsLoaded() || res->IsLoading())
                        {
                            res->m_DependencyCount++;
                            continue;
                        }

                        //@TODO: Might be better for resource loader to recieve a corresponding vector of data and size
                        // Allows for independent batching operations
                        fileMgr.LoadAsync(res->GetPath(), [=](bool success, void* data, size_t bytes)
                        {
                            m_resourceLoader.EnqueueResource(res, data, bytes, success);
                        });
                    }

                    // Blocking - waits for batched resources to be loaded
                    // @TODO: Might not be needed? Consider race condition for resource
                    // that gets streamed in succession before being fully loaded
                    m_resourceLoader.Finish();

                    if(item.Callback)
                        item.Callback(item.Resources);
                }
            }

        }
    }
}

ResourceLoader::ResourceLoader()
{
    m_workThread = std::thread(&ResourceLoader::Execute, this);
}

void ResourceLoader::EnqueueResource(Resource* resource, void* data, size_t bytes, bool success)
{
    std::unique_lock<std::mutex> m_queueLock(m_queueMutex);
    m_queue.emplace_back(resource, data, bytes, success);
}

void ResourceLoader::Finish()
{
    while(m_queue.size() > 0)
    { /* Do nothing - BLOCK */
    }
}

void ResourceLoader::Execute()
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex, std::defer_lock);
    while(1)
    {
        if(m_queue.size() > 0)
        {
            if(queueLock.try_lock())
            {
                if(m_queue.size() > 0)
                {
                    // CRITICAL
                    AsyncItem item = m_queue.front();
                    m_queue.pop_front();
                    // END CRITICAL

                    queueLock.unlock();

                    // Process Item
                    ASSERT(item.InResource, "Attempting to Load null resource");
                    item.InResource->OnLoaded(item.bSuccess, item.Data, item.NumBytes);
                }
            }

        }
    }
}