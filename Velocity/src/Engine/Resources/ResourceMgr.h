#pragma once

#include "Engine/Core/Core.h"
#include "Resource.h"

using LoadCallback = std::function<void(const void*, size_t)>;

struct FileLoadTask
{
    FileLoadTask(const char* filepath, LoadCallback onLoadComplete);
    ~FileLoadTask();

    void LoadData();

    const char* const Data() const { return m_Data; }
    size_t NumBytes() const { return m_Bytes; }
private:
    const char* m_File;
    char* m_Data;
    size_t m_Bytes;
    LoadCallback m_OnLoadComplete;
};

struct StreamableMgr
{
    void EnqueueResource(Resource* res, LoadCallback onLoadComplete);
    void EnqueueResource(const std::vector<Resource*>& res, LoadCallback onLoadComplete);
    void Finish(bool blocking = false);
private:
    void FinishInternal();

    std::vector<FileLoadTask> m_Tasks;
    std::thread m_WorkerThread;
};

class ResourceMgr
{
public:
    static std::string GetResourcePath(const char* res);
private:
    //std::unordered_map<StringId, Resource> m_ResourceMap;
};