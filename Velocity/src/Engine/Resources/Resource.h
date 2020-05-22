#pragma once

#include "Engine/Core/Types/Path.h"

enum class ResourceState
{
    UNLOADED = 0,
    LOADED,
    LOADING,
    INVALID
};

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

        bool operator==(const Resource::Type& Other) const
        {
            return (TypeId == Other.TypeId);
        }

        StringId TypeId;
        FORCEINLINE bool IsValid() const { return TypeId != StringId::NONE; }
    };

    Resource(const std::string& resFile);
    Resource(const Path& resPath);
    virtual ~Resource();

    Resource(const Resource& Other) = delete; // Cannot copy without modifying ref count!

    bool IsValid() const; // Whether we are pointing to a valid, loadable resource path
    void IncrementRefCount();
    void DecrementRefCount();

    /*  Called either explicitly for raw data or when file is finished reading from OnLoaded call 
    *   Constructs the memory representation of the resource.
    */
    virtual bool Load(const void* const rawBinary, size_t bytes) = 0;
    virtual void Unload() = 0;
    virtual Type GetResourceType() const = 0;

    #define RES_TYPE(type) \
    virtual Type GetResourceType() const override { return type; } \
    static Type GetStaticType() { return type; }

    FORCEINLINE bool        operator==(const Resource& Other) const { return m_ResPath == Other.m_ResPath; }
    FORCEINLINE const Path& GetPath() const { return m_ResPath; }
    FORCEINLINE uint32_t    GetRefCount() const { return m_RefCount; }
    FORCEINLINE bool        IsLoaded() const { return m_ResState == ResourceState::LOADED; }
    FORCEINLINE bool        IsLoading() const { return m_ResState == ResourceState::LOADING; }
    FORCEINLINE ResourceState GetState() const { return m_ResState; }
protected:
    /*  Starts loading the resource from the file pointed to by m_ResPath 
    *   Resource loading is asynchronous, and the OnLoaded method is used
    *   as callback to post-process the read file.
    */
    void BeginLoad(bool bBlocking = false);

    virtual void OnModified() {}
    virtual bool OnLoaded(bool success, const void* const data, size_t bytes);
    virtual void OnUnloaded();

private:
    Path m_ResPath;
    uint32_t m_RefCount;
    ResourceState m_ResState;
};

namespace std {
    template <> struct hash<Resource::Type>
    {
        size_t operator()(const Resource::Type& x) const
        {
            return hash<StringId>()(x.TypeId);
        }
    };
}

struct ResourcePtr
{
    ResourcePtr();
    ResourcePtr(const Path& path, const Resource::Type& type);
    ResourcePtr(Resource& Other);
    ResourcePtr(Resource* Other);
    ResourcePtr& operator=(Resource& Other);
    ResourcePtr& operator=(Resource* Other);

    virtual ~ResourcePtr();

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
    TResourcePtr(const Path& path)
        : ResourcePtr(path, T::GetStaticType())
    {
    }

    TResourcePtr(T& Other)
        : ResourcePtr(Other)
    {
    }

    TResourcePtr(T* Other)
        : ResourcePtr(Other)
    {
    }

    TResourcePtr& operator=(T& Other)
    {
        ResourcePtr::operator=(Other);
    }

    TResourcePtr& operator=(T* Other)
    {
        ResourcePtr::operator=(Other);
    }

    T* Load()
    {
        return dynamic_cast<T*>(ResourcePtr::Load());
    }

    T* Get() const
    {
        return dynamic_cast<T*>(ResourcePtr::Get());
    }
};