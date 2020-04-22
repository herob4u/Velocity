#pragma once
#include <stdint.h>

#define MAX_CRC_DIGEST 256

class CRC
{
public:
    /* HashBytes
    *   Returns a 32-bit unsigned integer hash for an array of bytes of length elements
    *
    *   HashString
    *   Returns a 32-bit unsigned integer hash for a string container or raw C string.
    *   If no length is specified, the string is hashed until a null terminator is found
    *   or if the string exceeds the maximum CRC digest size.
    */
    static uint32_t HashBytes(uint8_t* bytes, size_t length);
    static uint32_t HashString(const std::string& str);
    static uint32_t HashString(const char* str, size_t len = 0);
};