#pragma once

#include "Engine/Core/Core.h"
#include "Resource.h"

using OnResourcesLoaded = std::function<void(const std::vector<Resource*>&)>;

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

    ResourceStreamer();
    void EnqueueResources(const std::vector<Resource*>& resources, OnResourcesLoaded cb);

protected:
    void Execute();

    std::list<AsyncItem> m_queue;
    std::thread m_workThread;
    std::mutex m_queueMutex;
};

/*  The Resource Manager facilitates the loading, unloading, and reference counting 
*   of abstract resource types. Resource Managers are further template specialized to deal with
*   specific categories of resources.
*
*   Resources are created using a static factory method, see Resource.h
*/

class ResourceMgr
{
public:
    void Load(const Path& resPath);
    // This may be unecessary, file loading is inherently async, post processing result may be very fast, and not warrant async operation
    void LoadAsync(const std::vector<Path>& resPaths, OnResourcesLoaded cb);
    void Unload(const Path& resPath);

    void Dump() const;
protected:
    // These are called internally to handle the allocation and destruction of resources.
    // @TODO: Instead of virtual methods, how about template specialization?
    virtual Resource* CreateResource(const Path& resPath) = 0;
    virtual void DestroyResource(Resource& res) = 0;
    virtual short GetExtensionId(const std::string& extension) const = 0;

protected:
    typedef std::unordered_map<StringId, Resource*> ResourceList;
    ResourceList m_ResourceList;
    ResourceStreamer m_ResourceStreamer;
};

template <typename T, typename MgrClass>
class TResourceMgr : public ResourceMgr
{
public:
    static MgrClass& Get()
    {
        static MgrClass self;
        return self;
    }
};