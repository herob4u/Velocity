#include "vctPCH.h"
#include "FileStream.h"

static char s_TempChar;

InputFileStream::~InputFileStream()
{
    if(m_Handle)
        Close();
}

bool InputFileStream::Open(const char* filepath)
{
    auto err = fopen_s(&m_Handle, filepath, "rb");
    
    if(err != 0)
    {
        m_Handle = nullptr;
        return false;
    }

    return true;
}

bool InputFileStream::Open(const Path& filepath)
{
    return Open(filepath.GetFullPathRef());
}

void InputFileStream::Close()
{
    ASSERT(m_Handle, "File was never open!");
    fclose(m_Handle);
    m_Handle = nullptr;
}

bool InputFileStream::Read(void* data, size_t numBytes)
{
    ASSERT(m_Handle, "No open file to read!");
    //return (fread(data, sizeof(uint8_t), numBytes, m_Handle) == numBytes);
    const size_t read = fread(data, sizeof(uint8_t), numBytes, m_Handle);
    return (read == numBytes);
}

void* InputFileStream::Data() const
{
    return nullptr;
}

size_t InputFileStream::Size() const
{
    if(!m_Handle)
        return 0;

    fseek(m_Handle, 0, SEEK_END);
    size_t numBytes = ftell(m_Handle);
    fseek(m_Handle, 0, SEEK_SET);

    return numBytes;
}

bool InputFileStream::Seek(uint64_t offset)
{
    if(m_Handle)
    {
        int err = fseek(m_Handle, static_cast<long>(offset), SEEK_SET);
        return (err == 0);
    }
    return false;
}

bool InputFileStream::Offset(uint64_t offset)
{
    if(m_Handle)
    {
        int err = fseek(m_Handle, static_cast<long>(offset), SEEK_CUR);
        return (err == 0);
    }
    return false;
}

bool InputFileStream::SetPos(uint64_t pos)
{
    const fpos_t fpos = pos;
    return (fsetpos(m_Handle, &fpos) == 0);
}

uint64_t InputFileStream::GetOffset() const
{
    fpos_t pos;
    if(fgetpos(m_Handle, &pos) != 0)
    {
        return 0;
    }
    
    return (uint64_t)pos;
}

bool InputFileStream::IsOpen() const
{
    return (m_Handle != nullptr);
}

InputFileStream::operator bool() const
{
    return (m_Handle && feof(m_Handle) == 0);
}



OutputFileStream::~OutputFileStream()
{
    if(m_Handle)
        Close();
}

bool OutputFileStream::Open(const char* filepath)
{
    auto err = fopen_s(&m_Handle, filepath, "wb");
    
    if(err != 0)
    {
        m_Handle = nullptr;
        return false;
    }
    
    return true;
}

bool OutputFileStream::Open(const Path& filepath)
{
    return Open(filepath.GetFullPathRef());
}

void OutputFileStream::Close()
{
    ASSERT(m_Handle, "File was never open!");
    fclose(m_Handle);
    m_Handle = nullptr;
}

bool OutputFileStream::Write(const void* data, size_t numBytes)
{
    if(!m_Handle)
    {
        VCT_WARN("Write failed: No file open for write");
        return false;
    }

    return (fwrite(data, sizeof(char), numBytes, m_Handle) == numBytes);
}

OutputFileStream::operator bool() const
{
    return (m_Handle && feof(m_Handle) == 0);
}
