#pragma once

#include "Engine/Renderer/Material.h"

namespace Vct
{
    class Cubemap;

    class PBRMaterial final : public Material
    {
    public:
        PBRMaterial();

        MAT_VEC4_PARAM(DiffuseColor, "material.diffuse");
        MAT_VEC3_PARAM(EmissiveColor, "material.emissive");
        MAT_SCALAR_PARAM(MetallicIntensity, "material.metallic");
        MAT_SCALAR_PARAM(RoughnessIntensity, "material.roughness");

        MAT_TEXTURE2D_PARAM(DiffuseMap, 0, "maps.diffuse");
        MAT_TEXTURE2D_PARAM(NormalMap, 1, "maps.normal" );
        MAT_TEXTURE2D_PARAM(MaskMap, 2, "maps.mask1");
        MAT_CUBEMAP_PARAM(EnvironmentMap, 3, "maps.environment");
        MAT_CUBEMAP_PARAM(IrradianceMap, 4, "maps.irradiance");
        MAT_TEXTURE2D_PARAM(EmissiveMap, 5, "maps.emissive");

    private:
        void ResetToDefaults();
    private:
    };

    class PBRMaterialInstance final : public MaterialInstance
    {
    public:
        PBRMaterialInstance(PBRMaterial* parent);

        MAT_VEC4_PARAM(DiffuseColor, "material.diffuse");
        MAT_VEC3_PARAM(EmissiveColor, "material.emissive");
        MAT_SCALAR_PARAM(MetallicIntensity, "material.metallic");
        MAT_SCALAR_PARAM(RoughnessIntensity, "material.roughness");

        MAT_TEXTURE2D_PARAM(DiffuseMap, 0, "maps.diffuse");
        MAT_TEXTURE2D_PARAM(NormalMap, 1, "maps.normal");
        MAT_TEXTURE2D_PARAM(MaskMap, 2, "maps.mask1");
        MAT_CUBEMAP_PARAM(EnvironmentMap, 3, "maps.environment");
        MAT_CUBEMAP_PARAM(IrradianceMap, 4, "maps.irradiance");
        MAT_TEXTURE2D_PARAM(EmissiveMap, 5, "maps.emissive");
    };
}