#pragma once

#include "Mesh.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/Resources/Resource.h"

#include <vector>

namespace Vct
{
    static Resource::Type ResType_Model("model");

    class Texture2D;
    class Material;
    class MaterialInstance;

    /* A Model is an aggregate of meshes decomposed by materials. This exist to 
    *  describe meshes with multiple materials using a material id system.
    */
    class Model : public Resource
    {
    public:
        static Model* Import(const char* filepath);
        static Model* FromMesh(Mesh* mesh, MaterialInstance* material);
        static Model* FromMeshes(const std::vector<Mesh*>& meshes, const std::vector<MaterialInstance*>& materials);
        static Model* FromMeshes(std::vector<std::unique_ptr<Mesh>>& meshes, std::vector<MaterialInstance*>& materials); // Move constructor

        Model(const std::string& resFile);
        Model(const Path& resPath);

        void SetMaterial(uint8_t materialId, Material* material) {}

        Mesh* GetMesh(uint8_t meshId) const;
        MaterialInstance* GetMaterial(uint8_t materialId) const;

        RES_TYPE(ResType_Model);
    protected:
        /* Resource Interface */
        virtual bool Load(const void* rawBinary, size_t bytes) override;
        virtual void Unload() override;
        virtual void UpdateDependencies() override;
    private:
        // Model claims ownership of meshes
        std::vector<std::unique_ptr<Mesh>> m_Meshes;
        std::vector<MaterialInstance*> m_Materials;
    };
}