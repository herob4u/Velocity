#pragma once
#include "Stream.h"
#include "Engine/Core/Types/Path.h"

class InputFileStream final : public IInputStream
{
public:
    enum class Mode
    {
        BINARY
    };

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

    operator bool() const;
private:
    FILE* m_Handle;
    size_t m_Size;
};

class OutputFileStream final : public IOutputStream
{
public:

private:
    FILE* m_Handle;
};