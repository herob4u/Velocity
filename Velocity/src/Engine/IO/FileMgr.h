#pragma once

#include "Engine/Core/Types/Path.h"
#include "FileStream.h"
#include "FileTypes.h"


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

struct AsyncBatchItem
{
    AsyncBatchItem(const std::vector<Path>& filePaths, FileLoadedDelegate cb)
        : Callback(cb)
        , bCancelled(false)
    {
        FilePaths.reserve(filePaths.size());
        for(const Path& path : filePaths)
        {
            FilePaths.emplace_back(path);
        }
    }

    std::vector<Path> FilePaths;
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

    /*  Given a relative file path, returns an absolute path with respect to the manager's base/root path.
    *   This is rapdily faster than performing path concatenation using the + operator, the concatenated path
    *   is created in place on the stack, from which a new path is constructed and returned.
     */
    Path GetAbsPath(const Path& path) const;

    // Sync Operations
    bool LoadSync(std::fstream& file, void** outData, size_t& outNumBytes) const;
    bool LoadSync(InputFileStream& inputFile, void** outData, size_t& outNumBytes) const;
    bool Rename(const char* from, const char* to);
    bool Move(const char* from, const char* to);
    bool Copy(const char* from, const char* to);
    bool Exists(const char* filepath);
    bool DirectoryExists(const char* directory);
    bool Delete(const char* filepath);
    
    void GetFiles(const char* inDirectory, std::vector<std::string>& outFiles) const;
    void GetFiles(const char* inDirectory, std::vector<std::string>& outFiles, const std::string& extension) const;
    bool MakeDir(const char* directory);
    
    // Async Operations
    FileHandle LoadAsync(const Path& inputFilePath, FileLoadedDelegate onFileLoaded);
    void Cancel(FileHandle& handle);

    static FileMgr& Get();

private:
    void GetAbsPath(char outAbsPath[MAX_PATH_LENGTH], const char* file) const;
    bool CheckPermission(const char* filepath, const char* mode) const;
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

        std::list<struct AsyncItem> m_queue; // handles contain actual stream object, callback, and load status
        std::thread m_workThread;
        std::mutex m_queueMutex;
        InputFileStream m_FileStream;
        bool m_Finished = false;
        FileMgr& m_FileMgr;
    };

    FileIOTask m_Task;
    char m_BasePath[MAX_PATH_LENGTH];
};