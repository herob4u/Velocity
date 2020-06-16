#include "vctPCH.h"
#include "Material.h"

using namespace Vct;

static uint32_t s_MaterialID;

Material::Material(const std::string & resFile)
    : Resource(resFile)
    , m_Id(s_MaterialID++)
{
}

Material::Material(const Path & resPath)
    : Resource(resPath)
    , m_Id(s_MaterialID++)
{
}
