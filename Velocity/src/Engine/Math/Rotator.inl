#include "glm/trigonometric.hpp"

/* Yaw is -90 in order to face forward. Otherwise, 0 would begin in the Right direction */
FORCEINLINE Rotator::Rotator()
    : Pitch(0.f)
    , Yaw(-90.f)
    , Roll(0.f)
{
}

FORCEINLINE Rotator::Rotator(float pitch, float yaw, float roll)
    : Pitch(pitch)
    , Yaw(yaw)
    , Roll(roll)
{
}

inline Rotator Rotator::FromDirection(const glm::vec3 dir)
{
    return Rotator( glm::degrees(asin(dir.z)),
                    glm::degrees(atan2(dir.z, dir.x)),
                    0);

}

FORCEINLINE Rotator Rotator::Inverse() const
{
    return Rotator(-Pitch, -Yaw, - Roll);
}

FORCEINLINE glm::vec3 Rotator::ToDirection() const
{
    glm::vec3 direction;
    const float pitch   = glm::radians(Pitch);
    const float yaw     = glm::radians(Yaw);
    const float roll    = glm::radians(Roll);

    direction.x = cos(yaw) * cos(pitch);
    direction.y = -sin(pitch);
    direction.z = sin(yaw) * cos(pitch);

    return direction;
}

FORCEINLINE glm::mat4 Rotator::ToRotationMatrix() const
{
    // XYZ Order
    // Source: https://stackoverflow.com/questions/1568568/how-to-convert-euler-angles-to-directional-vector
    const float rotX = glm::radians(Pitch);
    const float rotY = glm::radians(Yaw);
    const float rotZ = glm::radians(Roll);

    const float    Sx = sinf(rotX);
    const float    Sy = sinf(rotY);
    const float    Sz = sinf(rotZ);
    const float    Cx = cosf(rotX);
    const float    Cy = cosf(rotY);
    const float    Cz = cosf(rotZ);

    return glm::mat4(Cy*Cz,             Cy*Sz,              Sy,     0.f,
                     Cz*Sx+Cx*Sz,       Cx*Cz-Sx*Sy*Sz,     -Cy*Sx, 0.f,
                     Cx*Cz*Sy+Sx*Sz,    Cz*Sx + Cx*Sy*Sz,   Cx*Cy,  0.f,
                     0.f,               0.f,                0.f,    1.f);
}

FORCEINLINE Rotator Rotator::operator+(const Rotator& Other)
{
    return Rotator( Pitch + Other.Pitch,
                    Yaw + Other.Yaw,
                    Roll + Other.Roll);
}

FORCEINLINE Rotator Rotator::operator-(const Rotator& Other)
{
    return Rotator( Pitch - Other.Pitch,
                    Yaw - Other.Yaw,
                    Roll - Other.Roll);
}

FORCEINLINE void Rotator::operator+=(const Rotator& Other)
{
    Pitch   += Other.Pitch;
    Yaw     += Other.Yaw;
    Roll    += Other.Roll;
}

FORCEINLINE void Rotator::operator-=(const Rotator& Other)
{
    Pitch   -= Other.Pitch;
    Yaw     -= Other.Yaw;
    Roll    -= Other.Roll;
}