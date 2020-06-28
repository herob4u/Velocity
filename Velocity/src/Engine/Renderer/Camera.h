#pragma once

#include "Engine/Math/Rotator.h"

namespace Vct
{
    class Camera
    {
    public:
        static const float WORLD_Z_MAX;
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
                , FarZ(WORLD_Z_MAX)
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
        Camera(float fov, float aspect, float nearz = 0.001f, float farz = WORLD_Z_MAX);

        void SetFOV(float fov);
        void SetAspect(float aspect);
        void SetNearZ(float nearz);
        void SetFarZ(float farz);
        void SetFrustum(const Frustum& frustum);

        FORCEINLINE float GetFOV() const                    { return m_Frustum.FOV; }
        FORCEINLINE float GetAspect() const                 { return m_Frustum.Aspect; }
        FORCEINLINE float GetNearZ() const                  { return m_Frustum.NearZ; }
        FORCEINLINE float GetFarZ() const                   { return m_Frustum.FarZ; }
        FORCEINLINE const Frustum& GetFrustum() const       { return m_Frustum;}

        /* Updates the aspect the ratio to match the screen/window */
        void NotifyScreenResized(int newWidth, int newHeight);

        const glm::vec3&    GetPosition() const { return m_Position; }
        const Rotator&      GetRotation() const { return m_Rotation; }

        void SetPosition(const glm::vec3& pos);
        void Offset(const glm::vec3& offset);
        void SetRotation(const Rotator& rotation);
        void AddRotation(const Rotator& rotation);
        void LookAt(const glm::vec3 target);

        const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
        const glm::mat4& GetView() const { return m_View; }
        const glm::mat4& GetProjection() const { return m_Projection; }
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