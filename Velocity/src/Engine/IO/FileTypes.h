#pragma once

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

    FileHandle(const FileHandle& Other)
        : Id(Other.Id)
    {
    }

    bool IsValid() const { return Id != StringId::NONE; }
    void Invalidate() { Id = StringId::NONE; }
    const StringId& GetId() const { return Id; }
private:
    StringId Id;
};