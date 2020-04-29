#include "vctPCH.h"
#include "ResourceMgr.h"

#include <fstream>

FileLoadTask::FileLoadTask(const char* filepath, LoadCallback onLoadComplete)
    : m_File(filepath)
    , m_Data(nullptr)
    , m_Bytes(0)
    , m_OnLoadComplete(onLoadComplete)
{
}

FileLoadTask::~FileLoadTask()
{
    if(m_Data)
        free(m_Data);
}

void FileLoadTask::LoadData()
{
//#define CPP_FSTREAM

#ifdef CPP_FSTREAM
    std::ifstream file(m_File, std::ios::binary);

    if(!file.good())
    {
        VCT_WARN("Failed to open file '{0}' . File does not exist or is corrupt.");
        return;
    }

    file.seekg(0, file.end);
    m_Bytes = file.tellg();
    file.seekg(0, file.beg);

    m_Data = (char*)malloc(sizeof(uint8_t) * m_Bytes);

    file.read(m_Data, m_Bytes);
    file.close();
#else
    FILE* file = nullptr;
    auto err = fopen_s(&file, m_File, "r");

    if(err != 0)
    {
        VCT_WARN("Failed to open file '{0}' . File does not exist or is corrupt.");
        return;
    }

    fseek(file, 0, SEEK_END);
    m_Bytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    m_Data = (char*)malloc(sizeof(char) * m_Bytes + 1);

    fread(m_Data, sizeof(char), m_Bytes, file);
    fclose(file);

    // Terminate file with null character
    m_Data[m_Bytes] = 0;

#endif // CPP_FSTREAM

    if(m_OnLoadComplete)
    {
        m_OnLoadComplete((const void*)m_Data, m_Bytes);
    }
}

void StreamableMgr::EnqueueResource(Resource* res, LoadCallback onLoadComplete)
{
    if(!res)
        return;

    Resource& resRef = *res;
    if(!resRef.IsValid())
    {
        VCT_WARN("Cannt load resource, not pointing to a valid resource path");
    }

    if(!resRef.IsLoaded())
    {
        const char* pathRef = resRef.GetPath().GetFullPathRef();
        if(!pathRef)
        {
            VCT_WARN("Cannot load resource, invalid path provided");
        }

        m_Tasks.emplace_back(FileLoadTask(pathRef, onLoadComplete));
    }
}

void StreamableMgr::EnqueueResource(const std::vector<Resource*>& res, LoadCallback onLoadComplete)
{
    m_Tasks.reserve(m_Tasks.size() + res.size() + 1);
    for(int i = 0; i < res.size(); i++)
    {
        EnqueueResource(res[i], onLoadComplete);
    }
}

void StreamableMgr::Finish(bool blocking)
{
    if(blocking)
        FinishInternal();
    else
    {
        m_WorkerThread = std::thread(&StreamableMgr::FinishInternal, this);
    }
    VCT_TRACE("[MAIN]: Finished Processing Tasks");
}

void StreamableMgr::FinishInternal()
{
    for(int i = 0; i < m_Tasks.size(); i++)
    {
        m_Tasks[i].LoadData();
    }
}

std::string ResourceMgr::GetResourcePath(const char* res)
{
    return std::string(ASSET_DIR) + std::string(res);
}
