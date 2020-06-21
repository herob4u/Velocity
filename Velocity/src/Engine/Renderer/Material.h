#pragma once

#include "Engine/Resources/Resource.h"

#include "ShaderProgram.h"

#include "glm/vec3.hpp"

namespace Vct
{
    static const Resource::Type ResType_Material("material");

    class Texture2D;

    class Material : public Resource
    {
    public:
        Material(const std::string& resFile);
        Material(const Path& resPath);

        FORCEINLINE uint32_t GetId() const { return m_Id; }
        FORCEINLINE const ShaderProgram* GetShader() const { return m_Shader.Get(); }

        RES_TYPE(ResType_Material);

    protected:
        // Resource Interface
        virtual bool Load(const void* rawBinary, size_t bytes) override { return false; }
        virtual void Unload() override { }
    private:
        TResourcePtr<ShaderProgram> m_Shader;
        uint32_t m_Id;
    };

    class StandardMaterial // : public Resource
    {
    public:
        void SetDiffuseColor(const glm::vec3& diffuseColor);
        void SetEmissiveColor(const glm::vec3& emissiveColor);
        void SetMetallicIntensity(float alpha);
        void SetRoughnessIntensity(float alpha);

        void SetDiffuseMap(const Texture2D* diffuseMap);
        void SetEmissiveMap(const Texture2D* emissiveMap);
        void SetNormalMap(const Texture2D* normalMap);
        void SetMetallicMap(const Texture2D* metallicMap);
        void SetRoughnessMap(const Texture2D* roughnessMap);
    protected:
    private:
    };
}