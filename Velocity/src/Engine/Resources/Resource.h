#pragma once

#include "Engine/Core/Types/Path.h"

class Resource
{
public:
    Resource(const std::string& resFile);
    virtual ~Resource();

    Resource(const Resource& Other) = delete; // Cannot copy without modifying ref count!

    bool IsValid() const; // Whether we are pointing to a valid, loadable resource path

    bool Load();
    void Unload();

    FORCEINLINE bool operator==(const Resource& Other) const { return m_ResPath == Other.m_ResPath; }
    FORCEINLINE const Path& GetPath() const { return m_ResPath; }
    FORCEINLINE uint32_t GetRefCount() const { return m_RefCount; }
    FORCEINLINE bool IsLoaded() const { return m_IsLoaded; }
protected:
    virtual void OnModified() = 0;
    virtual bool OnLoaded(const void* const data, size_t bytes) = 0;
    virtual void OnUnloaded() = 0;

private:
    Path m_ResPath;
    uint32_t m_RefCount;
    bool m_IsLoaded;
    bool m_PendingLoad;
};

template<typename T>
class TResource : public Resource
{
public:
    TResource(const std::string& resFile)
        : Resource(resFile)
    {
    }

    T* Get() const = 0;
};