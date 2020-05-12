#pragma once
#include "Stream.h"
#include "Engine/Core/Types/Path.h"

// @TODO: const correct some of the functions (i.e overloaded Read const that returns position after reading)
class InputFileStream final : public IInputStream
{
public:
    ~InputFileStream();
    bool Open(const char* filepath);
    bool Open(const Path& filepath);
    void Close();

    virtual bool Read(void* data, size_t numBytes) override;
    virtual void* Data() const override;
    virtual size_t Size() const override;

    bool Seek(uint64_t offset);
    bool Offset(uint64_t offset);
    bool SetPos(uint64_t pos);
    uint64_t GetOffset() const;

    bool IsOpen() const;
    operator bool() const;
private:
    FILE* m_Handle;
};

class OutputFileStream final : public IOutputStream
{
public:
    ~OutputFileStream();
    bool Open(const char* filepath);
    bool Open(const Path& filepath);
    void Close();

    virtual bool Write(const void* data, size_t numBytes) override;

private:
    FILE* m_Handle;
};