#include "vctPCH.h"
#include "Stream.h"

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
