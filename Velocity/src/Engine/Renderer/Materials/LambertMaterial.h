#pragma once

#include "Engine/Renderer/Material.h"

namespace Vct
{
    class LambertMaterial : public Material
    {
    public:
        LambertMaterial();
        ~LambertMaterial() {}

        MAT_MAT4_PARAM(ProjectionMatrix, "Projection");
        MAT_MAT4_PARAM(ViewMatrix, "View");
        MAT_MAT4_PARAM(ModelMatrix, "Model");

        MAT_VEC4_PARAM(Color, "material.color");
        MAT_VEC3_PARAM(Ambient, "material.ambient");

        MAT_VEC3_PARAM(LightPos, "light.position");
        MAT_VEC3_PARAM(LightColor, "light.intensity");
    };

    class LambertMaterialInstance : public MaterialInstance
    {
    public:
        LambertMaterialInstance(LambertMaterial* parent);
        ~LambertMaterialInstance() {}

        MAT_MAT4_PARAM(ProjectionMatrix, "Projection");
        MAT_MAT4_PARAM(ViewMatrix, "View");
        MAT_MAT4_PARAM(ModelMatrix, "Model");

        MAT_VEC4_PARAM(Color, "material.color");
        MAT_VEC3_PARAM(Ambient, "material.ambient");

        MAT_VEC3_PARAM(LightPos, "light.position");
        MAT_VEC3_PARAM(LightColor, "light.intensity");
    };
}