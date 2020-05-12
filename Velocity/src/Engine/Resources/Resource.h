#pragma once

#include "Engine/Core/Types/Path.h"

/* Resources are not acquired directly. Instead, client code uses ResourcePtr to represent assets */
class Resource
{
public:
    friend class ResourceMgr;
    friend struct ResourceStreamer;

    /* Usage: Each resource sub-class define */
    struct Type
    {
        Type()
            : TypeId(StringId::NONE)
        {
        }

        Type(const char* type)
            : TypeId(type)
        {
        }

        StringId TypeId;
        FORCEINLINE bool IsValid() const { return TypeId != StringId::NONE; }
    };

    Resource(const std::string& resFile);
    Resource(const Path& resPath);
    virtual ~Resource();

    Resource(const Resource& Other) = delete; // Cannot copy without modifying ref count!

    bool IsValid() const; // Whether we are pointing to a valid, loadable resource path

    /*  Called either explicitly for raw data or when file is finished reading from OnLoaded call 
    *   Constructs the memory representation of the resource.
    */
    virtual bool Load(const void* const rawBinary, size_t bytes) = 0;
    virtual void Unload() = 0;
    virtual Type GetResourceType() const { return Type(); }

    FORCEINLINE bool        operator==(const Resource& Other) const { return m_ResPath == Other.m_ResPath; }
    FORCEINLINE const Path& GetPath() const { return m_ResPath; }
    FORCEINLINE uint32_t    GetRefCount() const { return m_RefCount; }
    FORCEINLINE bool        IsLoaded() const { return m_IsLoaded; }
protected:
    /*  Starts loading the resource from the file pointed to by m_ResPath 
    *   Resource loading is asynchronous, and the OnLoaded method is used
    *   as callback to post-process the read file.
    */
    void BeginLoad(bool bBlocking = false);

    virtual void OnModified() {}
    virtual bool OnLoaded(bool success, const void* const data, size_t bytes);
    virtual void OnUnloaded() {}

private:
    Path m_ResPath;
    uint32_t m_RefCount;
    bool m_IsLoaded;
    bool m_PendingLoad;
};

struct ResourcePtr
{
    ResourcePtr(const Resource& Other);
    ResourcePtr(const Resource* Other);
    ResourcePtr& operator=(const Resource& Other);
    ResourcePtr& operator=(const Resource* Other);

    bool IsValid() const;
    bool IsNull() const;

    /* Returns the resource object pointed to by ResPath. Loads the resource synchronously if not already loaded */
    Resource* Load();

    /* Returns the resource object pointed to by ResPath if it is loaded. Null otherwise */
    Resource* Get() const;

    Path ResPath;
    Resource::Type ResType;
    Resource* ResPtr;
};

template <typename T>
struct TResourcePtr : public ResourcePtr
{
    T* Load()
    {
        return dynamic_cast<T*>(ResourcePtr::Load());
    }

    T* Get() const
    {
        return dynamic_cast<T*>(ResourcePtr::Get());
    }
};