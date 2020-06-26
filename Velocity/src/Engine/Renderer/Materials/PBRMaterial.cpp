#include "vctPCH.h"
#include "PBRMaterial.h"

using namespace Vct;

PBRMaterial::PBRMaterial()
    : Material(new Shader("PBR"))
{
    ResetToDefaults();
}

void PBRMaterial::ResetToDefaults()
{
    SetDiffuseColor(glm::vec3(0.5));
    SetEmissiveColor(glm::vec3(0.f));
    SetMetallicIntensity(0.5f);
    SetRoughnessIntensity(0.1f);
}
