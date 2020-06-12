#include "vctPCH.h"
#include "Resource.h"
#include "ResourceMgr.h"
#include "Engine/IO/FileMgr.h"

Resource::Resource(const std::string& resFile)
    : m_ResPath(resFile.c_str())
    , m_RefCount(0)
    , m_DependencyCount(0)
    , m_ResState(ResourceState::UNLOADED)
{
    FileMgr& mgr = FileMgr::Get();
    m_AbsPath = mgr.GetAbsPath(m_ResPath);
}

Resource::Resource(const Path& resPath)
    : m_ResPath(resPath)
    , m_RefCount(0)
    , m_DependencyCount(0)
    , m_ResState(ResourceState::UNLOADED)
{
    FileMgr& mgr = FileMgr::Get();
    m_AbsPath = mgr.GetAbsPath(m_ResPath);
}

Resource::~Resource()
{
    ASSERT((m_RefCount > 0U), "Invalid Ref Count. Is Resource lingering?");
    // Reference counting is handled in ResourcePtr
    /*
    m_RefCount--;

    if(m_RefCount == 0)
    {
        //Unload();
    }
    */
}

bool Resource::IsValid() const
{
    return true;
}

void Resource::IncrementRefCount()
{
    ++m_RefCount;
}

void Resource::DecrementRefCount()
{
    --m_RefCount;
    if(m_RefCount == 0)
    {
        // Last reference to this resource instance has been released, unload it!
        if(IsLoading() && m_LoadHandle.IsValid())
        {
             FileMgr& mgr = FileMgr::Get();
             mgr.Cancel(m_LoadHandle);
        }

        DoUnload();
    }
}

void Resource::DoUnload()
{
    Unload();
    OnUnloaded();
}

void Resource::BeginLoad(bool bBlocking)
{
    FileMgr& mgr = FileMgr::Get();
    if(!bBlocking)
    {
        m_ResState = ResourceState::LOADING;
        m_LoadHandle = mgr.LoadAsync(m_AbsPath, BIND_MFN_3(&Resource::OnLoaded, this));
    }
    else
    {
        InputFileStream file;
        file.Open(m_AbsPath);

        if(file)
        {
            m_ResState = ResourceState::LOADING;

            void* data = nullptr;
            size_t bytes = 0;
            bool success = mgr.LoadSync(file, &data, bytes);

            OnLoaded(success, data, bytes);
        }
    }
}

void Resource::NotifyDependencyLoad(Resource* dependency)
{
    if(dependency)
    {
        dependency->m_DependencyCount++;
        if(!dependency->IsLoaded() && !dependency->IsLoading())
        {
            dependency->BeginLoad();
        }
    }
}

void Resource::NotifyDependencyUnload(Resource* dependency)
{
    if(dependency)
    {
        dependency->m_DependencyCount--;
        if(dependency->m_DependencyCount == 0)
        {
            dependency->DoUnload();
        }
    }
}

void Resource::OnModified()
{
    if(m_ResState == ResourceState::LOADED)
    {
    }
    else if(m_ResState == ResourceState::UNLOADED)
    {
    }
}

bool Resource::OnLoaded(bool success, void* data, size_t bytes)
{
    m_LoadHandle.Invalidate();

    if(!success)
    {
        m_ResState = ResourceState::INVALID;
        free(data);
        return false;
    }

    if(Load(data, bytes))
    {
        m_ResState = ResourceState::LOADED;
        free(data);
        return true;
    }

    m_ResState = ResourceState::INVALID;
    free(data);
    return false;
}

void Resource::OnUnloaded()
{
    m_ResState = ResourceState::UNLOADED;
}

/*===========================================================
                        RESOURCE PTR
*===========================================================*/
ResourcePtr::ResourcePtr()
    : ResPath()
    , ResType()
    , ResPtr(nullptr)
{
}

ResourcePtr::ResourcePtr(const Path& path, const Resource::Type& type)
    : ResPath(path)
    , ResType(type)
    , ResPtr(nullptr)
{
    //ResPtr = StaticGetResource(type, path);
    ResPtr = StaticGetLoadedResource(type, path);
}

ResourcePtr::ResourcePtr(Resource& Other)
    : ResPath (Other.GetRelativePath())
    , ResType (Other.GetResourceType())
    , ResPtr(&Other)
{
    Other.IncrementRefCount();
}

ResourcePtr::ResourcePtr(Resource* Other)
    : ResPtr(Other)
{
    if(Other)
    {
        ResPath = Other->GetRelativePath();
        ResType = Other->GetResourceType();
        ResPtr->IncrementRefCount();
    }
    else
    {
        ResPath = StringId::NONE;
    }
}

ResourcePtr& ResourcePtr::operator=(Resource& Other)
{
    // Release existing resource
    if(ResPtr)
    {
        ResPtr->DecrementRefCount();
    }

    // Acquire new resource
    ResPath = Other.GetRelativePath();
    ResType = Other.GetResourceType();
    ResPtr  = &Other;
    
    Other.IncrementRefCount();

    return *this;
}

ResourcePtr& ResourcePtr::operator=(Resource* Other)
{
    if(ResPtr)
    {
        ResPtr->DecrementRefCount();
    }

    ResPtr = Other;

    if(ResPtr)
    {
        ResPath = Other->GetRelativePath();
        ResType = Other->GetResourceType();
        ResPtr->IncrementRefCount();
    }
    else
    {
        ResPath = StringId::NONE;
    }

    return *this;
}

ResourcePtr::~ResourcePtr()
{
    if(ResPtr)
    {
        ResPtr->DecrementRefCount();
    }
}

bool ResourcePtr::IsValid() const
{
    return !IsNull() && (ResPath.GetPathId() != StringId::NONE) && (ResPtr->IsLoaded());
}

bool ResourcePtr::IsNull() const
{
    return (ResPtr == nullptr);
}

Resource* ResourcePtr::Load()
{
    Resource* res = Get();
    if(!res || (!res->IsLoaded()) )
    {
        // ResourceMgr Load
        res = StaticLoadResource(ResType, ResPath);
    }

    ResPtr = res->IsLoaded() ? res : nullptr;

    return ResPtr;
}

Resource* ResourcePtr::Get() const
{
    return ResPtr;
}