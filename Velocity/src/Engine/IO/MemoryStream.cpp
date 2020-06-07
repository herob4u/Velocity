#include "vctPCH.h"
#include "MemoryStream.h"

InputMemoryStream::InputMemoryStream(void* buffer, size_t bytes)
    : m_Buffer(buffer)
    , m_Bytes(bytes)
    , m_ReadIt(0)
{
}

InputMemoryStream::InputMemoryStream(const void* const buffer, size_t bytes)
    : m_Buffer(const_cast<void*>(buffer))
    , m_Bytes(bytes)
    , m_ReadIt(0)
{
}

bool InputMemoryStream::Read(void* data, size_t numBytes)
{
    ASSERT(m_Buffer, "Invalid input buffer");
    ASSERT(m_ReadIt + numBytes <= m_Bytes, "Read buffer out of range");
    memcpy(data, m_Buffer, numBytes);
    m_ReadIt += numBytes;

    return true;
}
