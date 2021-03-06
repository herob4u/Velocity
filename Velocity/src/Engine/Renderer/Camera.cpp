#include "vctPCH.h"
#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

using namespace Vct;

const float Camera::WORLD_Z_MAX = 1000.f;

Camera::Camera(const Frustum& frustum)
    : m_Frustum(frustum)
    , m_View(glm::mat4(1.f))
    , m_ViewProjection(glm::mat4(1.f))
    , m_Position(glm::vec3(0.f))
    , m_Rotation(Rotator())
{
    RecomputeProjection();
}

Camera::Camera(float fov, float aspect, float nearz, float farz)
    : m_Frustum(Frustum(fov, aspect, nearz, farz))
    , m_View(glm::mat4(1.f))
    , m_ViewProjection(glm::mat4(1.f))
    , m_Position(glm::vec3(0.f))
    , m_Rotation(Rotator())
{
    RecomputeProjection();
}

void Camera::SetFOV(float fov)
{
    m_Frustum.FOV = fov;
    RecomputeProjection();
}

void Camera::SetAspect(float aspect)
{
    m_Frustum.Aspect = aspect;
    RecomputeProjection();
}

void Camera::SetNearZ(float nearz)
{
    m_Frustum.NearZ = nearz;
    RecomputeProjection();
}

void Camera::SetFarZ(float farz)
{
    m_Frustum.FarZ = farz;
    RecomputeProjection();
}

void Camera::SetFrustum(const Frustum& frustum)
{
    m_Frustum = frustum;
    RecomputeProjection();
}

void Camera::NotifyScreenResized(int newWidth, int newHeight)
{
    const float newAspect = (float)newWidth / (float)newHeight;
    SetAspect(newAspect);
}


void Camera::SetPosition(const glm::vec3& pos)
{
    m_Position = pos;
    RecomputeView();
}

void Camera::Offset(const glm::vec3& offset)
{
    m_Position += offset;
    RecomputeView();
}

void Camera::SetRotation(const Rotator& rotation)
{
    m_Rotation = rotation;

    if(m_Rotation.Pitch > 89.0f)
        m_Rotation.Pitch = 89.0f;
    else if(m_Rotation.Pitch < -89.0f)
        m_Rotation.Pitch = -89.0f;

    RecomputeView();
}

void Camera::AddRotation(const Rotator& rotation)
{
    m_Rotation += rotation;

    if(m_Rotation.Pitch > 89.0f)
        m_Rotation.Pitch = 89.0f;
    else if(m_Rotation.Pitch < -89.0f)
        m_Rotation.Pitch = -89.0f;

    RecomputeView();
}

void Camera::LookAt(const glm::vec3 target)
{
    const glm::vec3 dir = glm::normalize(target - m_Position);
    m_Rotation = Rotator::FromDirection(dir);

    RecomputeView();
}

void Camera::RecomputeProjection()
{
    m_Projection = glm::perspective(glm::radians(m_Frustum.FOV), m_Frustum.Aspect, m_Frustum.NearZ, m_Frustum.FarZ);
    RecomputeViewProjection();
}

void Camera::RecomputeView()
{
    //m_View = glm::translate(glm::mat4(1.f), -flippedPos) * m_Rotation.ToRotationMatrix();
    
    glm::vec3 direction = m_Rotation.ToDirection();

    // Target position to look at
    glm::vec3 lookat = m_Position + glm::normalize(direction);
    m_View = glm::lookAt(m_Position, lookat, glm::vec3(0.f, 1.f, 0.f));
    
    RecomputeViewProjection();
}

void Camera::RecomputeViewProjection()
{
    // OpenGL P V M convention
    m_ViewProjection = m_Projection * m_View;
}
