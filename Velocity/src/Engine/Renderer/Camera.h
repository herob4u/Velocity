#pragma once

#include "Engine/Math/Rotator.h"

namespace Vct
{
    class Camera
    {
    public:
        struct Frustum
        {
            float FOV;
            float Aspect;
            float NearZ;
            float FarZ;

            Frustum()
                : FOV(45.f)
                , Aspect(16.f/9.f)
                , NearZ(0.1f)
                , FarZ(10.f)
            {
            }

            Frustum(float fov, float aspect, float nearz, float farz)
                : FOV(fov)
                , Aspect(aspect)
                , NearZ(nearz)
                , FarZ(farz)
            {
            }
        };

        Camera(const Frustum& frustum = Frustum());
        Camera(float fov, float aspect, float nearz = 0.1f, float farz = 10.f);

        void SetFOV(float fov);
        void SetAspect(float aspect);
        void SetNearZ(float nearz);
        void SetFarZ(float farz);
        void SetFrustum(const Frustum& frustum);

        /* Updates the aspect the ratio to match the screen/window */
        void NotifyScreenResized(int newWidth, int newHeight);

        const glm::vec3&    GetPosition() const { return m_Position; }
        const Rotator&      GetRotation() const { return m_Rotation; }

        void SetPosition(const glm::vec3& pos);
        void Offset(const glm::vec3& offset);
        void AddRotation(const Rotator& rotation);
        void LookAt(const glm::vec3 target);

        const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
    protected:
        void RecomputeProjection();
        void RecomputeView();
        void RecomputeViewProjection();
    private:
        Frustum m_Frustum;

        Rotator m_Rotation;
        glm::vec3 m_Position;

        glm::mat4 m_Projection;
        glm::mat4 m_View;
        glm::mat4 m_ViewProjection;
    };
}