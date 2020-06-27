#pragma once

#include "Engine/Renderer/Camera.h"

namespace Vct
{
    class ICameraController
    {
    public:
        ICameraController(Camera& camera)
            : m_Camera(camera)
        {
        }

        virtual ~ICameraController() {}

        virtual void Update(float dt) = 0;

        virtual void OnMouseInputX(float dx) = 0;
        virtual void OnMouseInputY(float dy) = 0;
        virtual void OnZoomInput(float dz) = 0;

        virtual void OnBeginMoveX(float scalar) =0;
        virtual void OnBeginMoveY(float scalar) =0;
        virtual void OnBeginMoveZ(float scalar) =0;

        virtual void OnEndMoveX(float scalar) =0;
        virtual void OnEndMoveY(float scalar) =0;
        virtual void OnEndMoveZ(float scalar) =0;
        /*
        virtual void OnMoveInputX(float scalar) = 0;
        virtual void OnMoveInputY(float scalar) = 0;
        virtual void OnMoveInputZ(float scalar) = 0;
        */
    protected:
        Camera& m_Camera;
    };

    class FreeCameraController : ICameraController
    {
    public:
        FreeCameraController(Camera& camera);

        virtual void Update(float dt) override;

        virtual void OnMouseInputX(float dx) override;
        virtual void OnMouseInputY(float dy) override;
        virtual void OnZoomInput(float dz) override;

        virtual void OnBeginMoveX(float scalar) override;
        virtual void OnBeginMoveY(float scalar) override;
        virtual void OnBeginMoveZ(float scalar) override;

        virtual void OnEndMoveX(float scalar) override;
        virtual void OnEndMoveY(float scalar) override;
        virtual void OnEndMoveZ(float scalar) override;

        void SetMoveSpeed(float multiplier);
        void SetViewSpeed(float multiplier);
        void SetZoomSpeed(float multiplier);

        FORCEINLINE float GetMoveSpeed() const { return m_MoveSpeed; }
        FORCEINLINE float GetViewSpeed() const { return m_ViewSpeed; }
        FORCEINLINE float GetZoomSpeed() const { return m_ZoomSpeed; }

    protected:
        void OnDrag(const glm::vec2& delta) {}

        void ResetDefaults();
    private:
        float m_MoveSpeed;
        float m_ViewSpeed;
        float m_ZoomSpeed;

        glm::vec3 m_TargetVelocity;
        glm::vec3 m_CurrVelocity;
    };
}