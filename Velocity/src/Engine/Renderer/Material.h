#pragma once

#include "Engine/Resources/Resource.h"
#include "Shader.h"

#include "Texture/Texture2D.h"
#include "Texture/Cubemap.h"

#include "glm/glm.hpp"

namespace Vct
{
    static const Resource::Type ResType_Material("material");

    class Material : public Resource
    {
    public:
        enum Flags : uint8_t
        {
            NONE = 0,
            DOUBLE_SIDED = (1 << 0),
            WIREFRAME = (1 << 1),
        };

        Material(Shader* shader);

        Material(const std::string& resFile);
        Material(const Path& resPath);

        void Bind();
        void Unbind();
        void SetFlags(uint8_t flags);
        void SetFlag(Flags flag, bool on);

        FORCEINLINE uint32_t GetId() const      { return m_Id; }
        FORCEINLINE Shader* GetShader() const   { return m_Shader; }
        FORCEINLINE int GetFlags() const        { return m_Flags; }

        RES_TYPE(ResType_Material);

    protected:
        // Resource Interface
        virtual bool Load(const void* rawBinary, size_t bytes) override { return false; }
        virtual void Unload() override { }

        void SetTexture(int slot, Texture* texture);

        template <typename T>
        void SetUniform(const T&, const StringId& uniformName) { VCT_ERROR("Not implemented") }

        template <>
        void SetUniform<float>(const float& val, const StringId& uniformName) { m_Shader->SetUniform1f(uniformName, val); }

        template <>
        void SetUniform<glm::vec2>(const glm::vec2& val, const StringId& uniformName) { m_Shader->SetUniformVec2f(uniformName, val); }

        template <>
        void SetUniform<glm::vec3>(const glm::vec3& val, const StringId& uniformName) { m_Shader->SetUniformVec3f(uniformName, val); }

        template <>
        void SetUniform<glm::mat4>(const glm::mat4& val, const StringId& uniformName) { m_Shader->SetUniformMat4(uniformName, val); }
    private:
        Shader* m_Shader;
        uint32_t m_Id;
        uint8_t m_Flags;
        std::vector<Texture*> m_Textures;
    };

    /* A mutable instance of a material. Basically an independent copy of the parent material */
    class MaterialInstance
    {
    public:
        MaterialInstance(Material* parent);

        void Bind();
        void Unbind();

        void SetFlags(uint8_t flags);
        void SetFlag(Material::Flags flag, bool on);

        FORCEINLINE Material* GetMaterial() const { return m_Material; }
        FORCEINLINE uint32_t  GetId() const { return m_Id; }
        FORCEINLINE int GetFlags() const { return m_Flags; }
    protected:
        void SetTexture(int slot, Texture* texture);

        template <typename T>
        void SetUniform(const T&, const StringId& uniformName) { VCT_ERROR("Not implemented") }

        template <>
        void SetUniform<float>(const float& val, const StringId& uniformName) { m_Material->GetShader()->SetUniform1f(uniformName, val);}

        template <>
        void SetUniform<glm::vec2>(const glm::vec2& val, const StringId& uniformName) { m_Material->GetShader()->SetUniformVec2f(uniformName, val); }

        template <>
        void SetUniform<glm::vec3>(const glm::vec3& val, const StringId& uniformName) { m_Material->GetShader()->SetUniformVec3f(uniformName, val); }

        template <>
        void SetUniform<glm::mat4>(const glm::mat4& val, const StringId& uniformName) { m_Material->GetShader()->SetUniformMat4(uniformName, val); }
    private:
        Material* m_Material;
        uint32_t m_Id;
        uint8_t m_Flags;
        std::vector<Texture*> m_Textures;
    };
}

#define MAT_SCALAR_PARAM(ParamName, UniformName)\
void Set##ParamName(float alpha) { SetUniform(alpha, UniformName); }

#define MAT_VEC2_PARAM(ParamName, UniformName)\
void Set##ParamName(const glm::vec2& val) { SetUniform(val, UniformName); }

#define MAT_VEC3_PARAM(ParamName, UniformName)\
void Set##ParamName(const glm::vec3& val) { SetUniform(val, UniformName); }

#define MAT_MAT4_PARAM(ParamName, UniformName)\
void Set##ParamName(const glm::mat4& val) { SetUniform(val, UniformName); }

#define MAT_TEXTURE2D_PARAM(ParamName, Slot, UniformName)\
void Set##ParamName(Texture2D* texture) { SetTexture(Slot, texture); }

#define MAT_CUBEMAP_PARAM(ParamName, Slot, UniformName)\
void Set##ParamName(Cubemap* texture) { SetTexture(Slot, texture); }