#include "vctPCH.h"
#include "FileMgr.h"

#include <fstream>
#include <chrono>


void FileMgr::FileIOTask::EnqueueFile(FileHandle& fileHandle)
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex);
    m_queue.push_back(fileHandle);
}

void FileMgr::FileIOTask::Execute()
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex, std::defer_lock);

    while(true)
    {
        if(m_queue.size() > 0)
        {
            if(queueLock.try_lock())
            {
                // Verify if there are elements in the queue
                if(m_queue.size() > 0)
                {
                    FileHandle fileHandle = m_queue.front();
                    m_queue.pop_front();

                    FileContent outContent;
                    FileError err = fileHandle.Load(outContent);
                    ASSERT(err == FileError::NONE, "Error code {0} when trying to load file", err);

                    FileLoadedDelegate& cb = fileHandle.Callback;
                    
                    if(cb)
                        cb( (err == FileError::NONE), outContent);
                }
                queueLock.unlock();
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

bool FileHandle::IsLoaded() const
{
    return false;
}

bool FileHandle::CanRead() const
{
    return CheckPermission("r");
}

bool FileHandle::CanWrite() const
{
    return CheckPermission("w");
}

FileError FileHandle::Load(FileContent& outContent)
{
    FILE* file;
    auto err = fopen_s(&file, FilePath.GetFullPathRef(), "rb");
    
    if(!err)
    {
        fseek(file, 0, SEEK_END);
        size_t numBytes = ftell(file);
        fseek(file, 0, SEEK_SET);

        if(numBytes == 0)
            return FileError::FILE_EMPTY;

        char* data = (char*)malloc(sizeof(char) * numBytes + 1);

        fread(data, sizeof(char), numBytes, file);
        fclose(file);

        // Terminate file with null character
        data[numBytes] = 0;

        // Update file content with acquired info
        outContent.Data = data;
        outContent.NumBytes = numBytes;

        return FileError::NONE;
    }

    switch(err)
    {
        case ENOENT: return FileError::FILE_NOT_FOUND;
        case EPERM: return FileError::NO_READ_PERMISSION;
        default: return FileError::IO_FAILED;
    }
}

FileError FileHandle::Replace(const FileContent& content)
{
    if(content.NumBytes == 0 || content.Data == nullptr)
        return FileError::WRITEDATA_EMPTY;

    FILE* file;
    auto err = fopen_s(&file, FilePath.GetFullPathRef(), "wb");
    
    if(!err)
    {
        size_t written = fwrite(content.Data, sizeof(char), content.NumBytes, file);
        fclose(file);

        if(written < content.NumBytes)
            return FileError::INCOMPLETE_WRITE;
        
        return FileError::NONE;
    }

    switch(err)
    {
        case ENOENT: return FileError::FILE_NOT_FOUND;
        case EPERM: return FileError::NO_WRITE_PERMISSION;
        default: return FileError::IO_FAILED;
    }
}

FileError FileHandle::Swap(FileHandle& Other)
{
    const char* tmp = tmpnam(nullptr);
    auto err = rename(Other.FilePath.GetFullPathRef(), FilePath.GetFullPathRef());
    ASSERT(!err, "Error code {0} when renaming file '{1}'", err, Other.FilePath.GetFullPathRef());

    err = rename(FilePath.GetFullPathRef(), Other.FilePath.GetFullPathRef());
    ASSERT(!err, "Error code {0} when renaming file '{1}'", err, FilePath.GetFullPathRef());

    return FileError::NONE;
}

bool FileHandle::CheckPermission(const char* mode) const
{
    FILE* file = nullptr;
    auto err = fopen_s(&file, FilePath.GetFullPathRef(), mode);

    if(!err)
    {
        fclose(file);
        return true;
    }

    return false;
}

FileMgr::FileMgr()
{
}

FileHandle& FileMgr::GetFile(const Path& path)
{
    auto found = std::find(m_FileHandles.begin(), m_FileHandles.end(), [=](const FileHandle& A) { return A.GetFileId() == path.GetPathId(); });
    if(found != m_FileHandles.end())
    {
        return *found;
    }

    m_FileHandles.emplace_back(FileHandle(path, nullptr));
    return m_FileHandles.back();
}

FileMgr& FileMgr::Get()
{
    static FileMgr instance;
    return instance;
}


void FileMgr::FileReadTask::Execute()
{
}

void FileMgr::FileWriteTask::Execute()
{
}
