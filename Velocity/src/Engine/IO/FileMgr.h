#pragma once

#include "Engine/Core/Types/Path.h"
#include <deque>

struct FileContent
{
    FileContent()
        : Data(nullptr)
        , NumBytes(0)
    {
    }

    ~FileContent()
    {
        //Release();
    }

    void Release()
    {
        if(Data)
            free(Data);

        Data = nullptr;
        NumBytes = 0;
    }

    void* Data;
    size_t NumBytes;
};

/* FileLoadedDelegate(bool success, void* data, size_t numBytes) */
using FileLoadedDelegate = std::function<void(bool, FileContent)>;
using FileWriteDelegate  = std::function<void(bool)>;

enum class FileError
{
    NONE = 0,
    NO_READ_PERMISSION,
    NO_WRITE_PERMISSION,
    INCOMPLETE_WRITE,
    FILE_EMPTY,
    WRITEDATA_EMPTY,
    INVALID_SWAP_TARGET,
    FILE_NOT_FOUND,
    IO_FAILED // generic error msg
};

struct FileHandle
{
public:
    friend class FileMgr;

    FileHandle(const Path& path, FileLoadedDelegate onFileLoaded)
        : FilePath(path)
        , Callback(onFileLoaded)
        , bIsLoaded(false)
        , bPendingWrite(false)
        , bPendingRead(false)
    {
    }

    StringId GetFileId() const { return FilePath.GetPathId(); }
    bool IsLoaded() const;
    bool CanRead() const;
    bool CanWrite() const;

    /* Synchronous File Operations */
    FileError Load(FileContent& outContent);
    FileError Replace(const FileContent& content);
    FileError Swap(FileHandle& Other);

    bool operator==(const FileHandle& Other) { return FilePath == Other.FilePath; }
private:
    bool CheckPermission(const char* mode) const;

    Path FilePath;
    bool bIsLoaded;
    bool bPendingWrite;
    bool bPendingRead;
    FileLoadedDelegate Callback;
    FileWriteDelegate WriteCallback;
};

/* @TODO: Use FileHandles for synchronous operations exclusively. Async operations are done via the FileMgr 
   by providing the file path directly. A callback method then returns the handle or associated data */
class FileMgr
{
public:
    FileMgr();

    FileHandle& GetFile(const Path& path);
    static FileMgr& Get();

private:
private:
    std::vector<FileHandle> m_FileHandles;

    class FileIOTask
    {
        public:
        void EnqueueFile(FileHandle& fileHandle);
        virtual void Execute() = 0;

        private:
        std::deque<FileHandle&> m_queue; // handles contain actual stream object, callback, and load status
        std::thread m_workThread;
        std::mutex m_queueMutex;
        std::fstream m_FileStream;
    };

    class FileReadTask : public FileIOTask { virtual void Execute() override; };
    class FileWriteTask : public FileIOTask { virtual void Execute() override; };

    FileReadTask m_ReadTask;
    FileWriteTask m_WriteTask;
    //FileIOTask m_Task;
};