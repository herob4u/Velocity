#pragma once

#include "Mesh.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/Resources/Resource.h"

#include <vector>

class Texture;

static Resource::Type ResType_Model("model");

namespace Vct
{
    /* A Model is an aggregate of meshes decomposed by materials. This exist to 
    *  describe meshes with multiple materials using a material id system.
    */
    class Model : public Resource
    {
    public:
        static Model* Import(const char* filepath);

        Model(const std::string& resFile);
        Model(const Path& resPath);

        void SetMaterial(uint8_t materialId, Material* material) {}

        RES_TYPE(ResType_Model);
    protected:
        /* Resource Interface */
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;
        virtual void UpdateDependencies() override;
    private:
        /* TODO: Must be resources */
        std::vector<Mesh> m_Meshes;
        std::vector<TResourcePtr<Material>> m_Materials;
    };


    class ModelMgr : public ResourceMgr
    {
    public:

    protected:
        virtual Resource* CreateResource(const Path& resPath)
        {
            return new Model(resPath);
        }
    private:

    };
}