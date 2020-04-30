#pragma once

#include "Engine/Core/Types/Path.h"
#include <fstream>

/* FileLoadedDelegate(bool success, void* data, size_t numBytes) */
using FileLoadedDelegate = std::function<void(bool, void*, size_t)>;

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

    FileHandle(StringId id)
        : Id(id)
    {
    }

    FileHandle()
        : Id(StringId::NONE)
    {
    }

    bool IsValid() const { return Id != StringId::NONE; }
    const StringId Id;
};

struct AsyncItem
{
    AsyncItem(const Path& filePath, FileLoadedDelegate cb)
        : FilePath(filePath)
        , Callback(cb)
        , bCancelled(false)
    {
    }

    Path FilePath;
    FileLoadedDelegate Callback;
    bool bCancelled;
};

/* @TODO: Use FileHandles for synchronous operations exclusively. Async operations are done via the FileMgr 
   by providing the file path directly. A callback method then returns the handle or associated data */
class FileMgr
{
public:
    FileMgr();

    void SetBasePath(const char* basepath);

    FileHandle GetFile(const Path& path);
    FileHandle LoadAsync(const Path& inputFilePath, FileLoadedDelegate onFileLoaded);
    bool LoadSync(std::fstream& file, void** outData, size_t& outNumBytes);
    void Cancel(const FileHandle& handle);

    static FileMgr& Get();

private:
private:
    class FileIOTask
    {
        public:
        FileIOTask(FileMgr& owner);
        void EnqueueFile(const Path& fileHandle, FileLoadedDelegate onFileLoaded);
        void Cancel(const StringId& id);
        void Stop();

        protected:
        void Execute();

        private:
        std::list<struct AsyncItem> m_queue; // handles contain actual stream object, callback, and load status
        std::thread m_workThread;
        std::mutex m_queueMutex;
        std::fstream m_FileStream;
        bool m_Finished = false;
        FileMgr& m_FileMgr;
    };

    FileIOTask m_Task;
    Path m_BasePath;
};