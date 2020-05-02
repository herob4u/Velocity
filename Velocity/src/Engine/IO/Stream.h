#pragma once
#include "Engine/Core/Core.h"


class IInputStream
{
public:
    // Stream byte data of size numBytes into a buffer data
    virtual bool Read(void* data, size_t numBytes) = 0;
    virtual void* Data() const = 0;
    virtual size_t Size() const = 0;

    template<typename T>
    void Read(T&)
    {
        Read(&T, sizeof(T));
    }

    template<typename T>
    T Read()
    {
        T outVal;
        Read(&outVal, sizeof(T));
        return outVal;
    }

    IInputStream& operator >> (char* str);
    IInputStream& operator >> (char& c);
    IInputStream& operator >> (float& f);
    IInputStream& operator >> (double& d);
    IInputStream& operator >> (uint64_t& ui64);
    IInputStream& operator >> (uint32_t& ui32);
    IInputStream& operator >> (uint16_t& ui16);
    IInputStream& operator >> (int64_t& i64);
    IInputStream& operator >> (int32_t& i32);
    IInputStream& operator >> (int16_t& i16);
};

// Binary output stream
class IOutputStream
{
public:
    // Streams bytes found in data into some object with size numBytes
    virtual bool Write(const void* data, size_t numBytes) = 0;

    template<typename T>
    bool Write(const T& val)
    {
        return Write(&val, sizeof(T));
    }

    IOutputStream& operator << (char c);
    IOutputStream& operator << (const char* str);
    IOutputStream& operator << (uint64_t ui64);
    IOutputStream& operator << (uint32_t ui32);
    IOutputStream& operator << (uint16_t ui16);
    IOutputStream& operator << (int64_t i64);
    IOutputStream& operator << (int32_t i32);
    IOutputStream& operator << (int16_t i16);
    IOutputStream& operator << (float f);
    IOutputStream& operator << (double d);
};