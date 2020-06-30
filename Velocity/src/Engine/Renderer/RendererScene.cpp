#include "vctPCH.h"
#include "RendererScene.h"

#include "Lighting/Light.h"
#include "Lighting/PointLight.h"
#include "Lighting/DirectionalLight.h"

#include "glm/mat4x4.hpp"

#define MAX_SCENE_LIGHTS 16

namespace Vct
{
    // Must match the GLSL structure, this will be our Uniform Buffer Object
    struct SceneData
    {
        glm::mat4 View;
        glm::mat4 Projection;
        glm::mat3 NormalMatrix;

        Light Sunlight;
        Light Lights[16];
    };

    struct RendererSceneImpl : public RendererScene
    {

    };
}