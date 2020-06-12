#include "vctPCH.h"
#include "ResourceMgr.h"

#include "Engine/IO/FileMgr.h"

Resource* ResourceMgr::GetResource(const Path& resPath)
{
    auto found = m_ResourceList.find(resPath.GetPathId());
    if(found == m_ResourceList.end())
    {
        auto pair = m_ResourceList.emplace(resPath.GetPathId(), CreateResource(resPath));
        ASSERT(pair.second, "Failed to create resource from path '{0}'", resPath.GetFullPathRef());

        return pair.first->second;
    }

    Resource* res = found->second;
    ASSERT(res, "Resource was never successfully allocated!");
    res->IncrementRefCount();
    return res;
}

void ResourceMgr::Load(const Path& resPath, bool bLazyLoad)
{
    auto found = m_ResourceList.find(resPath.GetPathId());
    if(found == m_ResourceList.end())
    {
        Resource* res = CreateResource(resPath);
        ASSERT(res, "Failed to instantiate resource from '{0}'", resPath.GetFullPathRef());
        m_ResourceList.emplace(resPath.GetPathId(), res);
        res->BeginLoad(!bLazyLoad);
    }
    else
    {
        Resource* res = found->second;
        ASSERT(res, "An allocated resource can never be null");
        
        if(!res->IsLoaded() && !res->IsLoading())
            res->BeginLoad(!bLazyLoad);
    }
}

void ResourceMgr::Load(Resource& res, bool bLazyLoad)
{
    if(res.IsLoading() || res.IsLoaded())
        return;

    res.BeginLoad(!bLazyLoad);
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

        res->DoUnload();
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
                        if(res)
                            res->BeginLoad(true);
                    }

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
    { /* Do nothing - BLOCK */ }
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


void ResourceMgrRegistry::Register(const Resource::Type& type, ResourceMgr* mgr)
{
    ASSERT(m_ResourceMgrs.find(type) == m_ResourceMgrs.end(), "Resource Manager already registered for this type");
    m_ResourceMgrs.emplace(type, mgr);
}

void ResourceMgrRegistry::Shutdown()
{
    auto it = m_ResourceMgrs.begin();
    while(it != m_ResourceMgrs.end())
    {
        if(it->second)
        {
            delete (it->second);
        }

        m_ResourceMgrs.erase(it);
        ++it;
    }
}

ResourceMgr* ResourceMgrRegistry::GetMgr(const Resource::Type& type) const
{
    auto found = m_ResourceMgrs.find(type);
    if(found != m_ResourceMgrs.end())
    {
        return found->second;
    }

    VCT_WARN("Resource manager for type '{0}' not found!", type.TypeId.ToStringRef());
    return nullptr;
}

ResourceMgrRegistry& ResourceMgrRegistry::Get()
{
    static ResourceMgrRegistry self;
    return self;
}

Resource* StaticLoadResource(const Resource::Type& type, const Path& path)
{
    ResourceMgr* mgr = ResourceMgrRegistry::Get().GetMgr(type);
    ASSERT(mgr, "Cannot load resource, no associated resource manager");

    Resource* res = mgr->GetResource(path);
    if(res)
    {
        mgr->Load(*res, false);
    }

    return res;
}

Resource* StaticGetResource(const Resource::Type& type, const Path& path)
{
    ResourceMgr* mgr = ResourceMgrRegistry::Get().GetMgr(type);
    ASSERT(mgr, "Cannot load resource, no associated resource manager");

    Resource* res = mgr->GetResource(path);
    return res;
}

Resource* StaticGetLoadedResource(const Resource::Type& type, const Path& path)
{
    ResourceMgr* mgr = ResourceMgrRegistry::Get().GetMgr(type);
    ASSERT(mgr, "Cannot load resource, no associated resource manager");
    
    Resource* res = mgr->GetResource(path);
    
    if(res && !res->IsLoaded())
    {
        return nullptr;
    }

    return res;
}
