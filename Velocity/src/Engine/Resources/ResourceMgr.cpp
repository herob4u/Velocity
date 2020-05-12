#include "vctPCH.h"
#include "ResourceMgr.h"

#include "Engine/IO/FileMgr.h"

void ResourceMgr::Load(const Path& resPath)
{
    auto found = m_ResourceList.find(resPath.GetPathId());
    if(found == m_ResourceList.end())
    {
        Resource* res = CreateResource(resPath);
        ASSERT(res, "Failed to instantiate resource from '{0}'", resPath.GetFullPathRef());
        m_ResourceList.emplace(resPath.GetPathId(), res);
        res->BeginLoad();
    }
    else
    {
        Resource* res = found->second;
        ASSERT(res, "An allocated resource can never be null");
        
        if(!res->IsLoaded())
            res->BeginLoad();
    }
}

void ResourceMgr::LoadAsync(const std::vector<Path>& resPaths, OnResourcesLoaded cb)
{
    FileMgr& mgr = FileMgr::Get();
    std::vector<Resource*> resources;
    resources.reserve(resPaths.size());

    for(const Path& path : resPaths)
    {
        auto found = m_ResourceList.find(path.GetPathId());

        // @TODO: Need to first check if path leads to a valid file on disk...
        if(found == m_ResourceList.end())
        {
            // Allocate a new resource instance
            auto pair = m_ResourceList.emplace(path.GetPathId(), CreateResource(path));
            ASSERT(pair.second, "Failed to insert resource");
            resources.emplace_back(pair.first->second);
        }
        else
        {
            resources.emplace_back(found->second);
        }
    }
    // Streamable Mgr Enqueue ... Calls Resource->BeginLoad(true);
    m_ResourceStreamer.EnqueueResources(resources, cb);
}

void ResourceMgr::Unload(const Path& resPath)
{
    auto found = m_ResourceList.find(resPath.GetPathId());
    ASSERT(found != m_ResourceList.end(), "Attempting to unload a resource that never existed. Should not enter here");

    if(found != m_ResourceList.end())
    {
        Resource* res = found->second;
        ASSERT(res, "An allocated resource can never be null");

        res->Unload();
    }
}

void ResourceMgr::Dump() const
{
    if(m_ResourceList.size() == 0)
        VCT_INFO("ResourceMgr empty");

    for(auto it = m_ResourceList.begin(); it != m_ResourceList.end(); it++)
    {
        VCT_INFO("[ResId]: {0}", it->first.ToStringRef());
        VCT_INFO("[Resource]: {0}", ((void*)it->second));
    }
}

ResourceStreamer::ResourceStreamer()
{
    m_workThread = std::thread(&ResourceStreamer::Execute, this);
}

void ResourceStreamer::EnqueueResources(const std::vector<Resource*>& resources, OnResourcesLoaded cb)
{
    // @TODO: Which is faster - emplacing within a critical section, or constructing the object, entering the critical section, and pushing the item?
    // Latter seems to do AT LEAST twice the work of emplacing. So in effect, we will do the same amount of work in the critical section.
    // Pick the former.
    std::unique_lock<std::mutex> queueLock(m_queueMutex);
    m_queue.emplace_back(resources, cb);
}

void ResourceStreamer::Execute()
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex);
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
                        res->BeginLoad(true);
                    }

                    if(item.Callback)
                        item.Callback(item.Resources);
                }
            }

        }
    }
}
