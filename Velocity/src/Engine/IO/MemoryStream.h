#pragma once
#include "Stream.h"

/* Input memory stream that wraps around a binary buffer */
class InputMemoryStream : public IInputStream
{
public:
    InputMemoryStream(void* buffer, size_t bytes);
    InputMemoryStream(const void* const buffer, size_t bytes);

    virtual bool Read(void* data, size_t numBytes) override;
    virtual void* Data() const override { return m_Buffer; }
    virtual size_t Size() const override { return m_Bytes; }

private:
    void* m_Buffer;
    size_t m_Bytes;
    size_t m_ReadIt;
};

class OutputMemoryStream : public IOutputStream
{

};