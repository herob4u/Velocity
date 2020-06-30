#pragma once
#include "Engine/Core/IScene.h"
#include "Engine/Resources/Resource.h"

#include "glm/vec3.hpp"

namespace Vct
{
    class Camera;
    class Renderer;
    class Texture;
    class Cubemap;
    struct Light;

    struct Environment
    {
        std::unique_ptr<Cubemap> Skybox;
        glm::vec3 AmbientColor;
        glm::vec3 FogColor;
        float AmbientIntensity;
        float FogDensity;
        float FogHeight;
        float FogBottom;
    };

    // Reflection Probe that spans a square area of 2*Extent. Resolution is a power of 2 that dictates the dimension of the cubemap texture target
    struct ReflectionProbe
    {
        std::unique_ptr<Cubemap> Texture;
        glm::vec3 Extent;
        uint16_t Resolution;
    };

    struct RendererScene : public IScene
    {
        static RendererScene* Create(Renderer& renderer);
        static void Destroy(RendererScene* scene);

        virtual Camera& GetCamera() = 0;
        virtual ReflectionProbe& GetClosestReflectionProbe(const glm::vec3& fromPosition) const = 0;

        virtual Environment& GetEnvironmentSettings() = 0;
    };
}