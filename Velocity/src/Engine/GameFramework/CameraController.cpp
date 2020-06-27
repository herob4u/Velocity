#include "vctPCH.h"
#include "CameraController.h"

#include "Engine/Events/Input.h"

using namespace Vct;

FreeCameraController::FreeCameraController(Camera& camera)
    : ICameraController(camera)
    , m_TargetVelocity(glm::vec3(0.f))
    , m_CurrVelocity(glm::vec3(0.f))
{
    ResetDefaults();
}

void FreeCameraController::Update(float dt)
{
    // Testing out an inverse distance function to close the gap between interpolating quantities the closer they are
    const float dist = glm::length(m_TargetVelocity - m_CurrVelocity);
    float inv_dist = 1.f;
    if(dist > 0.01f && dist < 2.f)
    {
        inv_dist = 1.f / dist;
    }

    m_CurrVelocity = glm::mix(m_CurrVelocity, m_TargetVelocity, inv_dist * dt);

    glm::vec3 forward = m_Camera.GetRotation().ToDirection();
    glm::vec3 right = glm::cross(forward, glm::vec3(0.f, 1.f, 0.f));

    forward *= m_CurrVelocity.z;
    right *= m_CurrVelocity.x;

    m_Camera.Offset((forward + right));
}

void FreeCameraController::OnMouseInputX(float dx)
{
    // Yaw rotation
    Rotator delta = Rotator(0.f, dx * m_ViewSpeed, 0.f);
    m_Camera.AddRotation(delta);
}

void FreeCameraController::OnMouseInputY(float dy)
{
    // Pitch rotation
    Rotator delta = Rotator(dy * m_ViewSpeed, 0.f, 0.f);
    m_Camera.AddRotation(delta);
}

void FreeCameraController::OnZoomInput(float dz)
{
    const float fov = m_Camera.GetFOV();
    m_Camera.SetFOV(fov + m_ZoomSpeed);
}

void Vct::FreeCameraController::OnBeginMoveX(float scalar)
{
    m_TargetVelocity.x = scalar * m_MoveSpeed;
}

void Vct::FreeCameraController::OnBeginMoveY(float scalar)
{
    m_TargetVelocity.y = scalar * m_MoveSpeed;
}

void Vct::FreeCameraController::OnBeginMoveZ(float scalar)
{
    m_TargetVelocity.z = scalar * m_MoveSpeed;
}

void Vct::FreeCameraController::OnEndMoveX(float scalar)
{
    m_TargetVelocity.x = 0;
}

void Vct::FreeCameraController::OnEndMoveY(float scalar)
{
    m_TargetVelocity.y = 0;
}

void Vct::FreeCameraController::OnEndMoveZ(float scalar)
{
    m_TargetVelocity.z = 0;
}

void FreeCameraController::SetMoveSpeed(float multiplier)
{
    m_MoveSpeed = multiplier;
}

void FreeCameraController::SetViewSpeed(float multiplier)
{
    m_ViewSpeed = multiplier;
}

void FreeCameraController::SetZoomSpeed(float multiplier)
{
    m_ZoomSpeed = multiplier;
}

void FreeCameraController::ResetDefaults()
{
    m_MoveSpeed = 1.f;
    m_ViewSpeed = 0.05f;
    m_ZoomSpeed = 0.1f;
}
