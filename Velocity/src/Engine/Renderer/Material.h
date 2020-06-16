#pragma once

#include "Engine/Resources/Resource.h"

#include "ShaderProgram.h"

#include "glm/vec3.hpp"

namespace Vct
{
    class Texture;

    class Material : public Resource
    {
    public:
        Material(const std::string& resFile);
        Material(const Path& resPath);

        FORCEINLINE uint32_t GetId() const { return m_Id; }
        FORCEINLINE const ShaderProgram* GetShader() const { return m_Shader.Get(); }
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

        void SetDiffuseMap(const Texture* diffuseMap);
        void SetEmissiveMap(const Texture* emissiveMap);
        void SetNormalMap(const Texture* normalMap);
        void SetMetallicMap(const Texture* metallicMap);
        void SetRoughnessMap(const Texture* roughnessMap);
    protected:

    private:
    };
}