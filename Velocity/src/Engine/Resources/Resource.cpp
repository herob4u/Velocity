#include "vctPCH.h"
#include "Resource.h"

#include "Engine/IO/FileMgr.h"

Resource::Resource(const std::string& resFile)
    : m_ResPath(resFile.c_str())
    , m_RefCount(1)
    , m_IsLoaded(false)
{
}

Resource::Resource(const Path& resPath)
    : m_ResPath(resPath)
    , m_RefCount(1)
    , m_IsLoaded(false)
{
}

Resource::~Resource()
{
    ASSERT((m_RefCount > 0U), "Invalid Ref Count. Is Resource lingering?");
    m_RefCount--;

    if(m_RefCount == 0)
    {
        // Unload
    }
}

bool Resource::IsValid() const
{
    return true;
}

void Resource::BeginLoad(bool bBlocking)
{
    FileMgr& mgr = FileMgr::Get();
    if(!bBlocking)
    {
        mgr.LoadAsync(m_ResPath, BIND_MFN_3(&Resource::OnLoaded, this));
    }
    else
    {
        InputFileStream file;
        file.Open(m_ResPath);

        if(file)
        {
            void* data = nullptr;
            size_t bytes = 0;
            bool success = mgr.LoadSync(file, &data, bytes);

            OnLoaded(success, data, bytes);
        }
    }
}

/*===========================================================
                        RESOURCE PTR
 *===========================================================*/
ResourcePtr::ResourcePtr(const Resource& Other)
    : ResPath (Other.GetPath())
    , ResType (Other.GetResourceType())
{
}

ResourcePtr::ResourcePtr(const Resource* Other)
{
    if(Other)
    {
        ResPath = Other->GetPath();
        ResType = Other->GetResourceType();
    }
    else
    {
        ResPath = StringId::NONE;
    }
}

ResourcePtr& ResourcePtr::operator=(const Resource& Other)
{
    ResPath = Other.GetPath();
    ResType = Other.GetResourceType();
    return *this;
}

ResourcePtr& ResourcePtr::operator=(const Resource* Other)
{
    if(Other)
    {
        ResPath = Other->GetPath();
        ResType = Other->GetResourceType();
    }
    else
    {
        ResPath = StringId::NONE;
    }

    return *this;
}

bool ResourcePtr::IsValid() const
{
    return false;
}

bool ResourcePtr::IsNull() const
{
    // Ask ResourceMgr if 
    return false;
}

Resource* ResourcePtr::Load()
{
    Resource* res = Get();
    if(!res)
    {
        // ResourceMgr Load
        res = Get();
    }

    return res;
}

Resource* ResourcePtr::Get() const
{
    return ResPtr;
}
