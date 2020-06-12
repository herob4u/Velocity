#pragma once

#include "Mesh.h"

#include "Engine/Resources/Resource.h"

#include <vector>

namespace Vct
{
    /* A Model is an aggregate of meshes decomposed by materials. This exist to 
    *  describe meshes with multiple materials using a material id system.
    */
    class Model : public Resource
    {
    public:
        static Model* Import(const char* filepath);
        void SetMaterial(uint8_t materialId);
    private:
        /* TODO: Must be resources */
        std::vector<Mesh> m_Meshes;
        std::vector<Material*> m_Materials;
    };
}