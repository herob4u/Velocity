#include "vctPCH.h"
#include "Model.h"

#include "Engine/Renderer/Texture/Texture2D.h"

using namespace Vct;

Model* Model::Import(const char * filepath)
{
    return nullptr;
}

Model* Model::FromMesh(Mesh* mesh, MaterialInstance* material)
{
    Model* model = new Model(StringId::NONE);
    model->m_Meshes.emplace_back(mesh);
    model->m_Materials.emplace_back(material);

    return model;
}

Model* Model::FromMeshes(const std::vector<Mesh*>& meshes, const std::vector<MaterialInstance*>& materials)
{
    Model* model = new Model(StringId::NONE);
    ASSERT(meshes.size() == materials.size(), "Mesh and material IDs must associate");

    for(int i = 0; i < meshes.size(); i++)
    {
        model->m_Meshes.push_back(std::unique_ptr<Mesh>(meshes[i]));
        model->m_Materials.push_back(materials[i]);
    }

    return model;
}

Model* Model::FromMeshes(std::vector<std::unique_ptr<Mesh>>& meshes, std::vector<MaterialInstance*>& materials)
{
    Model* model = new Model(StringId::NONE);
    model->m_Meshes     = std::move(meshes);
    model->m_Materials  = std::move(materials);

    return model;
}

Model::Model(const std::string& resFile)
    : Resource(resFile)
{
}

Model::Model(const Path& resPath)
    : Resource(resPath)
{
}

Mesh* Model::GetMesh(uint8_t meshId) const
{
    if(meshId >= m_Meshes.size())
        return nullptr;

    return m_Meshes[meshId].get();
}

MaterialInstance* Model::GetMaterial(uint8_t materialId) const
{
    if(materialId >= m_Materials.size())
        return nullptr;

    return m_Materials[materialId];
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
