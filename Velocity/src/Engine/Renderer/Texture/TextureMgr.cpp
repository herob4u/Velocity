#include "vctPCH.h"
#include "TextureMgr.h"

#include "Texture.h"

Resource* TextureMgr::CreateResource(const Path& resPath)
{
    // @TODO: Parse extension and store format hint for proper loading here
    return new Vct::Texture(resPath);
}

void TextureMgr::DestroyResource(Resource& res)
{
    if(res.IsLoaded())
    {
        Unload(res.GetPath());
    }

    ASSERT(res.GetRefCount() == 0, "Resource still in use!");
    auto found = m_ResourceList.find(res.GetPath().GetPathId());
    if(found != m_ResourceList.end())
    {
        Resource* res = found->second;
        if(res)
        {
            delete res;
            res = nullptr;
        }

        m_ResourceList.erase(found);
    }
}
