#include "vctPCH.h"
#include "Resource.h"

Resource::Resource(const std::string& resFile)
    : m_ResPath(resFile.c_str())
    , m_RefCount(1)
    , m_IsLoaded(false)
{
}

Resource::~Resource()
{
    ASSERT((m_RefCount > 0U), "Invalid Ref Count. Is Resource lingering?");
    m_RefCount--;

    if(m_RefCount == 0)
    {
        // Unload
    }
}

bool Resource::IsValid() const
{
    return true;
}

bool Resource::Load()
{
    if(m_IsLoaded)
    {
        return true;
    }
    
    return false;
}

void Resource::Unload()
{
}
