#pragma once

#include "Engine/Core/Core.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

struct Rotator
{
    float Pitch;
    float Yaw;
    float Roll;

    Rotator();
    Rotator(float pitch, float yaw, float roll);

    static Rotator FromDirection(const glm::vec3 direction);

    Rotator Inverse() const;
    glm::vec3 ToDirection() const;
    glm::mat4 ToRotationMatrix() const;

    Rotator operator+(const Rotator& Other);
    Rotator operator-(const Rotator& Other);
    void operator+=(const Rotator& Other);
    void operator-=(const Rotator& Other);

    friend std::ostream& operator<<(std::ostream& os, const Rotator& rotator)
    {
        os << "{Pitch: " << rotator.Pitch << ", Yaw: " << rotator.Yaw << ", Roll: " << rotator.Roll << "}";
        return os;
    }
};

#include "Rotator.inl"