#include "vctPCH.h"
#include "FileMgr.h"

#include <fstream>
#include <chrono>



FileMgr::FileIOTask::FileIOTask(FileMgr& owner)
    : m_FileMgr(owner)
{
    m_workThread = std::thread(&FileIOTask::Execute, this);
}

void FileMgr::FileIOTask::EnqueueFile(const Path& filePath, FileLoadedDelegate onFileLoaded)
{
    // Wouldnt this cause the main thread to hang?
    std::unique_lock<std::mutex> queueLock(m_queueMutex);
    m_queue.emplace_back(filePath, onFileLoaded);
}

void FileMgr::FileIOTask::Cancel(const StringId& id)
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex);

    for(auto it = m_queue.begin(); it != m_queue.end(); it++)
    {
        if(it->FilePath.GetPathId() == id)
        {
            VCT_WARN("Cancelled file '{0}'", id.ToStringRef());
            it->bCancelled = true;
            return;
        }
    }
}

void FileMgr::FileIOTask::Execute()
{
    std::unique_lock<std::mutex> queueLock(m_queueMutex, std::defer_lock);

    while(!m_Finished)
    {
        if(m_queue.size() > 0)
        {
            if(queueLock.try_lock())
            {
                // Verify if there are elements in the queue
                if(m_queue.size() > 0)
                {
                    // <CRITICAL>
                    AsyncItem item = m_queue.front();
                    m_queue.pop_front();
                    // </CRITICAL>

                    // Critical section is done, can process the data without the mutex
                    queueLock.unlock();

                    // Items prematurely cancelled by the user are ignored and discared when popped.
                    if(!item.bCancelled)
                    {
                        m_FileStream.open(item.FilePath.GetFullPathRef(), std::ios::in);
                        ASSERT(m_FileStream.good(), "Failed to open file '{0}'", item.FilePath.GetFullPathRef());

                        void* data = nullptr;
                        size_t numBytes = 0;
                        bool loaded = m_FileMgr.LoadSync(m_FileStream, &data, numBytes);

                        // Execute the callback method if applicable (Why wouldn't there be a callback, useless work)
                        if(item.Callback)
                        {
                            item.Callback(loaded, data, numBytes);
                        }
                        
                        m_FileStream.close();
                    }
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void FileMgr::FileIOTask::Stop()
{
    m_Finished = true;
}

#if 0
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
#endif

FileMgr::FileMgr()
    : m_Task(*this)
{
}

void FileMgr::SetBasePath(const char* basepath)
{
    size_t len = strlen(basepath);
    if(len >= MAX_PATH_LENGTH)
    {
        ASSERT(false, "Path length exceeds maximum allowable length");
        return;
    }

    memcpy(m_BasePath, basepath, len);
}

Path FileMgr::GetAbsPath(const Path& path) const
{
    // Initialized to \0
    char absPath[MAX_PATH_LENGTH];
    const char* relPath = path.GetFullPathRef();

    GetAbsPath(absPath, relPath);

    return Path(absPath);
}

FileHandle FileMgr::LoadAsync(const Path& inputFilePath, FileLoadedDelegate onFileLoaded)
{
    //const Path newPath = (m_BasePath + inputFilePath);
    //m_Task.EnqueueFile(newPath, onFileLoaded);
    m_Task.EnqueueFile(inputFilePath, onFileLoaded);
    return FileHandle(inputFilePath.GetPathId());
    //return FileHandle(newPath.GetPathId());
}

bool FileMgr::LoadSync(std::fstream& file, void ** outData, size_t& outNumBytes) const
{
    if(!outData)
    {
        VCT_WARN("No target buffer found");
        return false;
    }
    ASSERT(!*outData, "Expected empty buffer");
    ASSERT(file.is_open(), "No currently open file");
    if(!file.is_open() || !file.good())
    {
        VCT_WARN("File could not be opened");
        return false;
    }

    file.seekg(0, std::ios::end);
    outNumBytes = file.tellg();
    file.seekg(0, std::ios::beg);

    // Reserve memory for data
    *outData = malloc(sizeof(char) * outNumBytes);
    file.read((char*)*outData, outNumBytes);

    return true;
}

bool FileMgr::LoadSync(InputFileStream& inputFile, void** outData, size_t& outNumBytes) const
{
    if(!outData)
    {
        VCT_WARN("No target buffer found");
        return false;
    }

    ASSERT(!*outData, "Expected empty buffer");
    
    // File not open/does not exist
    if(!inputFile)
        return false;

    outNumBytes = inputFile.Size();
    
    // Nothing to load
    if(outNumBytes == 0)
        return false;

    *outData = malloc(sizeof(char) * outNumBytes);
    return inputFile.Read(*outData, outNumBytes);
}

bool FileMgr::Rename(const char* from, const char* to)
{
    auto err = rename(from, to);
    return (err == 0);
}

bool FileMgr::Move(const char* from, const char* to)
{
    return Rename(from, to);
}

bool FileMgr::Copy(const char* from, const char* to)
{
    std::fstream file(from, std::ios::in);
    if(!file)
    {
        return false;
    }

    void* data = nullptr;
    size_t bytes = 0;

    bool success = LoadSync(file, &data, bytes);

    if(!success)
    {
        return false;
    }
    file.close();
    file.open(to, std::ios::out);
    ASSERT(file.good(), "INVALID OUTPUT FILE!");

    file.write((char*)data, bytes);
    file.close();

    free(data);
    
    return true;
}

bool FileMgr::Exists(const char* filepath)
{
    return CheckPermission(filepath, "r");
}

bool FileMgr::Delete(const char* filepath)
{
    auto err = remove(filepath);
    return (err == 0);
}

void FileMgr::Cancel(const FileHandle& handle)
{
    m_Task.Cancel(handle.Id);
}

FileMgr& FileMgr::Get()
{
    static FileMgr instance;
    return instance;
}

void FileMgr::GetAbsPath(char outAbsPath[MAX_PATH_LENGTH], const char* relPath) const
{
    int srcPtr = 0;
    int destPtr = 0;

    // We guarantee m_BasePath is exclusively less than MAX_PATH_LENGTH, no need to check here
    while(m_BasePath[srcPtr] != '\0')
    {
        outAbsPath[destPtr++] = m_BasePath[srcPtr++];
    }

    // Reset the source pointer to begin iterating over the second string
    srcPtr = 0;

    // Add to the abs path while ensuring it doesn't exceed the maximum length
    while(relPath[srcPtr] != '\0')
    {
        ASSERT(destPtr < (MAX_PATH_LENGTH-1), "Path exceeds maximum allowable length");
        outAbsPath[destPtr++] = relPath[srcPtr++];
    }

    outAbsPath[destPtr] = '\0';
}

bool FileMgr::CheckPermission(const char* filepath, const char* mode) const
{
    if(!filepath || !mode)
        return false;

    FILE* file = nullptr;
    auto err = fopen_s(&file, filepath, mode);

    if(!err)
    {
        fclose(file);
        return true;
    }

    return false;
}
