#pragma once

#include "Engine/Core/Core.h"
#include "Resource.h"
#include "ResourceQueues.h"


/*  The Resource Manager facilitates the loading, unloading, and reference counting 
*   of abstract resource types. Resource Managers are further specialized to deal with
*   certain categories of resource by a corresponding Resource::Type.
*
*   Resources are created by a specialized implementation of an associated ResourceMgr.
*   See ResourceMgr::CreateResource and ResourceMgr::DestroyResource
*/
namespace Vct
{
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

        template <typename T>
        T* GetResource(const Path& resPath)
        {
            ResourceMgr* mgr = GetMgr(T::GetStaticType());
            ASSERT(mgr, "Cannot load resource, no associated resource manager");

            return mgr->GetResource<T>(resPath);
        }

        static ResourceMgrRegistry& Get();
    private:
        std::unordered_map<Resource::Type, ResourceMgr*> m_ResourceMgrs;
    };
}

Vct::Resource* StaticLoadResource(const Vct::Resource::Type& type, const Path& path);
Vct::Resource* StaticGetResource(const Vct::Resource::Type& type, const Path& path);
Vct::Resource* StaticGetLoadedResource(const Vct::Resource::Type& type, const Path& path);