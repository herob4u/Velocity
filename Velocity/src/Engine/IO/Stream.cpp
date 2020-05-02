#include "vctPCH.h"
#include "Stream.h"

/* INPUT STREAM */
IInputStream& IInputStream::operator >> (char* str)
{
    if(str)
    {
        size_t length = strlen(str);
        Read(str, length);
    }

    return *this;
}

IInputStream& IInputStream::operator >> (char& c)
{
    Read(&c, sizeof(char));
    return *this;
}

IInputStream& IInputStream::operator >> (float& f)
{
    Read(&f, sizeof(float));
    return *this;
}

IInputStream& IInputStream::operator >> (double& d)
{
    Read(&d, sizeof(double));
    return *this;
}

IInputStream& IInputStream::operator >> (uint64_t& ui64)
{
    Read(&ui64, sizeof(uint64_t));
    return *this;
}

IInputStream& IInputStream::operator >> (uint32_t& ui32)
{
    Read(&ui32, sizeof(uint32_t));
    return *this;
}

IInputStream& IInputStream::operator >> (uint16_t& ui16)
{
    Read(&ui16, sizeof(uint16_t));
    return *this;
}

IInputStream& IInputStream::operator >> (int64_t& i64)
{
    Read(&i64, sizeof(int64_t));
    return *this;
}

IInputStream& IInputStream::operator >> (int32_t& i32)
{
    Read(&i32, sizeof(int32_t));
    return *this;
}

IInputStream& IInputStream::operator >> (int16_t& i16)
{
    Read(&i16, sizeof(int16_t));
    return *this;
}


/* OUTPUT STREAM */
IOutputStream& IOutputStream::operator<<(char c)
{
    Write(&c, sizeof(c));
    return *this;
}

IOutputStream& IOutputStream::operator<<(const char* str)
{
    Write(str, strlen(str));
    return *this;
}

IOutputStream& IOutputStream::operator<<(uint64_t ui64)
{
    Write(ui64);
    return *this;
}

IOutputStream& IOutputStream::operator<<(uint32_t ui32)
{
    Write(ui32);
    return *this;
}

IOutputStream& IOutputStream::operator<<(uint16_t ui16)
{
    Write(ui16);
    return *this;
}

IOutputStream& IOutputStream::operator<<(int64_t i64)
{
    Write(i64);
    return *this;
}

IOutputStream& IOutputStream::operator<<(int32_t i32)
{
    Write(i32);
    return *this;
}

IOutputStream& IOutputStream::operator<<(int16_t i16)
{
    Write(i16);
    return *this;
}

IOutputStream& IOutputStream::operator<<(float f)
{
    Write(f);
    return *this;
}

IOutputStream& IOutputStream::operator<<(double d)
{
    Write(d);
    return *this;
}
