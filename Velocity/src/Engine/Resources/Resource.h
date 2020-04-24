#pragma once

#include "Engine/Core/Types/StringId.h"

class Resource
{
public:
    Resource(const std::string& resFile);
    Resource(uint8_t bytes, size_t size);

    bool operator==(const Resource& Other);
private:
    StringId m_ResPath;
    uint32_t m_RefCount;
};