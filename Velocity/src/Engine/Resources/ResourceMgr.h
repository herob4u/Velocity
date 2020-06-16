#pragma once

#include "Engine/Core/Core.h"
#include "Resource.h"

using OnResourcesLoaded = std::function<void(const std::vector<Resource*>&)>;

/*  The ResourceStreamer accepts a list of assets to batch stream and load from disk 
*   Loading is deferred to the ResourceLoader to synchronize the loading of dependencies
*   while avoiding race conditions when modifying resource internal states.
*/
struct ResourceStreamer
{
    struct AsyncItem
    {
        AsyncItem(const std::vector<Resource*>& resources, OnResourcesLoaded cb)
            : Resources(resources)
            , Callback(cb)
        {
        }

        std::vector<Resource*> Resources;
        OnResourcesLoaded Callback;
    };

    ResourceStreamer(ResourceLoader& resourceLoader);
    void EnqueueResources(const std::vector<Resource*>& resources, OnResourcesLoaded cb);

protected:
    void Execute();

    std::list<AsyncItem> m_queue;
    std::thread m_workThread;
    std::mutex m_queueMutex;
    ResourceLoader& m_resourceLoader;
};

/* The resource loader is responsible for loading a single batch of stream requests */
struct ResourceLoader
{
    struct AsyncItem
    {
        AsyncItem(Resource* resource, void* data, size_t bytes, bool success)
            : InResource(resource)
            , Data(data)
            , NumBytes(bytes)
            , bSuccess(success)
        {
        }

        Resource* InResource;
        void* Data;
        size_t NumBytes;
        bool bSuccess;
    };

    ResourceLoader();
    void EnqueueResource(Resource* resource, void* data, size_t bytes, bool success);
    void Finish();

protected:
    void Execute();

    std::list<AsyncItem> m_queue;
    std::thread m_workThread;
    std::mutex m_queueMutex;
};

/*  The Resource Manager facilitates the loading, unloading, and reference counting 
*   of abstract resource types. Resource Managers are further specialized to deal with
*   certain categories of resource by a corresponding Resource::Type.
*
*   Resources are created by a specialized implementation of an associated ResourceMgr.
*   See ResourceMgr::CreateResource and ResourceMgr::DestroyResource
*/

class ResourceMgr
{
public:
    ResourceMgr();
    virtual ~ResourceMgr() {}

    void Load(const Path& resPath, bool bLazyLoad = true);
    void Load(Resource& res, bool bLazyLoad = true);

    // This may be unecessary, file loading is inherently async, post processing result may be very fast, and not warrant async operation
    void LoadAsync(const std::vector<Path>& resPaths, OnResourcesLoaded cb);
    void Unload(const Path& resPath);

    Resource* GetResource(const Path& resPath);

    template <typename T>
    T* GetResource(const Path& resPath)
    {
        return dynamic_cast<T*>(GetResource(resPath));
    }

    void Dump() const;
protected:
    // These are called internally to handle the allocation and destruction of resources.
    // @TODO: Instead of virtual methods, how about template specialization?
    virtual Resource* CreateResource(const Path& resPath) = 0;
    virtual void DestroyResource(Resource& res);

protected:
    typedef std::unordered_map<StringId, Resource*> ResourceList;
    ResourceList m_ResourceList;
    
    // These need to be static! Only 1 streamer and loader in the scope of the engine
    static ResourceStreamer m_ResourceStreamer;
    static ResourceLoader m_ResourceLoader;
};

class ResourceMgrRegistry
{
public:
    void Register(const Resource::Type& type, ResourceMgr* mgr);
    void Shutdown();
    ResourceMgr* GetMgr(const Resource::Type& type) const;

    static ResourceMgrRegistry& Get();
private:
    std::unordered_map<Resource::Type, ResourceMgr*> m_ResourceMgrs;
};

Resource* StaticLoadResource(const Resource::Type& type, const Path& path);
Resource* StaticGetResource(const Resource::Type& type, const Path& path);
Resource* StaticGetLoadedResource(const Resource::Type& type, const Path& path);