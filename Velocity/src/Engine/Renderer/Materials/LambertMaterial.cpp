#include "vctPCH.h"
#include "LambertMaterial.h"

using namespace Vct;

LambertMaterial::LambertMaterial()
    : Material(new Shader("Lambert"))
{
}

LambertMaterialInstance::LambertMaterialInstance(LambertMaterial* parent)
    : MaterialInstance(parent)
{
}
