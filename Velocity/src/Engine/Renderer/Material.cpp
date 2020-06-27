#include "vctPCH.h"
#include "Material.h"

#include "Texture/Texture.h"

using namespace Vct;

static uint32_t s_MaterialID;

Material::Material(Shader* shader)
    : Resource(StringId::NONE)
    , m_Shader(shader)
    , m_Id(s_MaterialID++)
{
}

Material::Material(const std::string& resFile)
    : Resource(resFile)
    , m_Id(s_MaterialID++)
{
}

Material::Material(const Path& resPath)
    : Resource(resPath)
    , m_Id(s_MaterialID++)
{
}

void Material::Bind()
{
    m_Shader->Bind();
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(Texture* texture = m_Textures[i])
        {
            texture->Bind(i);
        }
    }
}

void Material::Unbind()
{
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(Texture* texture = m_Textures[i])
        {
            texture->Unbind(i);
        }
    }

    m_Shader->Unbind();
}

void Material::SetFlags(uint8_t flags)
{
    m_Flags = flags;
}

void Material::SetFlag(Flags flag, bool on)
{
    on  ? m_Flags |= flag 
        : m_Flags &= ~(flag);
}

void Material::SetSceneUniforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
    SetUniform(model, "Model");
    SetUniform(view, "View");
    SetUniform(projection, "Projection");
}

void Material::SetTexture(int slot, Texture* texture)
{
    if(slot >= m_Textures.size())
    {
        m_Textures.resize(slot + 1);
    }

    m_Textures[slot] = texture;
}

MaterialInstance::MaterialInstance(Material* parent)
    : m_Material(parent)
    , m_Id(s_MaterialID++)
{
    ASSERT(m_Material, "MaterialInstance must be initialized with valid parent material");
    m_Flags = m_Material->GetFlags();
}

void MaterialInstance::Bind()
{
    m_Material->Bind();

    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(Texture* texture = m_Textures[i])
            texture->Bind(i);
    }
}

void MaterialInstance::Unbind()
{
    m_Material->Unbind();

    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(Texture* texture = m_Textures[i])
            texture->Unbind(i);
    }
}

void MaterialInstance::SetFlags(uint8_t flags)
{
    m_Flags = flags;
}

void MaterialInstance::SetFlag(Material::Flags flag, bool on)
{
    on  ? m_Flags |= flag
        : m_Flags &= ~(flag);
}

void MaterialInstance::SetSceneUniforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
    SetUniform(model, "Model");
    SetUniform(view, "View");
    SetUniform(projection, "Projection");
}

void MaterialInstance::SetTexture(int slot, Texture* texture)
{
    if(slot >= m_Textures.size())
    {
        m_Textures.resize(slot + 1);
    }

    m_Textures[slot] = texture;
}
