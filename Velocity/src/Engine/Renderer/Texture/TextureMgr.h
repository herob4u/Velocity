#pragma once
#include "Engine/Resources/ResourceMgr.h"

class TextureMgr : public ResourceMgr
{
public:

protected:
    Resource* CreateResource(const Path& resPath) override;
    void      DestroyResource(Resource& res) override;
};