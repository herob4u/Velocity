#include "vctPCH.h"
#include "Model.h"

#include "Engine/Renderer/Texture/Texture2D.h"

using namespace Vct;

Model* Model::Import(const char * filepath)
{
    return nullptr;
}

Model::Model(const std::string& resFile)
    : Resource(resFile)
{
}

Model::Model(const Path& resPath)
    : Resource(resPath)
{
}

bool Model::Load(const void* rawBinary, size_t bytes)
{
    ResourceMgr* mgr = ResourceMgrRegistry::Get().GetMgr(Texture2D::GetStaticType());

    return true;
}

void Model::Unload()
{
    ResourceMgr* mgr = ResourceMgrRegistry::Get().GetMgr(Model::GetStaticType());
}

void Model::UpdateDependencies()
{
    ResourceState state = GetState();

    if(state == ResourceState::LOADED)
    {
        //NotifyDependencyLoad(m_Texture);
    }
    else if(state == ResourceState::UNLOADED)
    {
        //NotifyDependencyUnload(m_Texture);
    }
}
