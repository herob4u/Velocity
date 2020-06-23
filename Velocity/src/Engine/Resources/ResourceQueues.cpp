#include "vctPCH.h"
#include "ResourceQueues.h"

#include "Resource.h"

#include "Engine/IO/FileMgr.h"

using namespace Vct;

ResourceStreamer::ResourceStreamer(SyncResourceLoader& resourceLoader)
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

                    // Create a batch load request for these resources
                    int batchId = m_resourceLoader.MakeNewLoadBatch(item.Resources.size(), item.Callback);

                    // Process Item
                    for(Resource* res : item.Resources)
                    {
                        if(res->IsLoaded() || res->IsLoading())
                        {
                            res->m_DependencyCount++;
                            continue;
                        }

                        //@TODO: Might be better for resource loader to recieve a corresponding vector of data and size
                        // Allows for independent batching operations
                        fileMgr.LoadAsync(res->GetPath(), [=](bool success, void* data, size_t bytes)
                        {
                            m_resourceLoader.EnqueueResource(batchId, res, data, bytes, success);
                        });
                    }

                    #ifdef ASYNC_LOADER
                    // Blocking - waits for batched resources to be loaded
                    // @TODO: Might not be needed? Consider race condition for resource
                    // that gets streamed in succession before being fully loaded
                    m_resourceLoader.Finish();

                    if(item.Callback)
                        item.Callback(item.Resources);

                    #endif // ASYNC_LOADER
                }
            }

        }
    }
}


/*========================================================================*/
//                          ASYNC RESOURCE LOADER
/*========================================================================*/
AsyncResourceLoader::AsyncResourceLoader()
{
    m_workThread = std::thread(&AsyncResourceLoader::Execute, this);
}

void AsyncResourceLoader::Execute()
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

void AsyncResourceLoader::EnqueueResource(Resource* resource, void* data, size_t bytes, bool success)
{
    std::unique_lock<std::mutex> m_queueLock(m_queueMutex);
    m_queue.emplace_back(resource, data, bytes, success);
}

void AsyncResourceLoader::Finish()
{
    while(m_queue.size() > 0)
    { /* Do nothing - BLOCK */
    }
}
/*========================================================================*/
//                          SYNC RESOURCE LOADER
/*========================================================================*/

SyncResourceLoader::SyncResourceLoader()
{
}

void SyncResourceLoader::EnqueueResource(int batchId, Resource* resource, void * data, size_t numBytes, bool success)
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex);

    ASSERT(batchId < SYNC_LOAD_BATCH_SIZE && batchId >= 0, "Invalid batch id provided");
    ASSERT(m_batchHeader[batchId].Remaining != 0, "Expected an initialized batch");

    m_queue[batchId].emplace_back(resource, data, numBytes, success);
}

int SyncResourceLoader::MakeNewLoadBatch(size_t batchSize, OnResourcesLoaded OnBatchLoaded)
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex, std::defer_lock);
    int id = -1;

    while(id == -1)
    {
        if(queueLock.try_lock())
        {
            id = GetNextBatchId();
            queueLock.unlock();
        }
    }

    queueLock.lock();

    // Found a batch ID, initialize!
    ASSERT(m_queue[id].size() == 0, "Expected empty queue");
    m_batchHeader[id].Remaining = batchSize;
    m_batchHeader[id].Callback  = OnBatchLoaded;

    queueLock.unlock();

    return id;
}

void SyncResourceLoader::Handle()
{
    std::unique_lock<std::mutex> m_queueLock(m_queueMutex);

    for(int i = 0; i < SYNC_LOAD_BATCH_SIZE; ++i)
    {
        if(m_queue[i].size() > 0)
        {
            ASSERT(m_batchHeader[i].Remaining > 0, "Corrupt batch, size mismatch");

            // Retrieve an item to process
            SyncItem item = m_queue[i].front();
            m_queue[i].pop_front();

            // Explicitly Load
            item.InResource->OnLoaded(item.bSuccess, item.Data, item.NumBytes);

            m_batchHeader[i].Remaining--;
            
            // Is this batch completed? Finalize it and invoke the callback
            if(m_batchHeader[i].Remaining == 0)
            {
                FinishBatch(i);
            }
        }
    }
}

int SyncResourceLoader::GetNextBatchId() const
{
    for(int i = 0; i < SYNC_LOAD_BATCH_SIZE; ++i)
    {
        if(m_batchHeader[i].Remaining == 0)
            return i;
    }

    return -1;
}

void SyncResourceLoader::FinishBatch(int batchId)
{
    BatchHeader& header = m_batchHeader[batchId];

    if(header.Callback)
    {
        header.Callback(header.Loaded);
    }

    header.Callback = nullptr;
    header.Loaded.clear();

    ASSERT(header.Remaining == 0, "Expected no remaining items");
    ASSERT(m_queue[batchId].size() == 0, "Expected no remaining items");
}
