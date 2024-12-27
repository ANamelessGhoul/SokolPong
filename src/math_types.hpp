
#pragma once

#ifndef MATH_TYPES_H
#define MATH_TYPES_H

#include "HandmadeMath.h"

typedef HMM_Vec2 Vector2;
typedef HMM_Vec3 Vector3;
typedef HMM_Vec4 Vector4;
typedef HMM_Vec4 Color;
typedef HMM_Mat4 Matrix;

constexpr Vector2 Vector2Zero = {0, 0};
constexpr Vector2 Vector2Up = {0, 1.f};
constexpr Vector2 Vector2Down = {0, -1.f};
constexpr Vector2 Vector2Right = {1.f, 0};
constexpr Vector2 Vector2Left = {-1.f, 0};

inline constexpr Color ColorFromHex(uint32_t hex)
{
    Color color;
    color.A = ((hex >>  0) & 0xFF) / 255.f;
    color.B = ((hex >>  8) & 0xFF) / 255.f;
    color.G = ((hex >> 16) & 0xFF) / 255.f;
    color.R = ((hex >> 24) & 0xFF) / 255.f;
    return color;
}

namespace Colors
{
    constexpr Color White   = {1.f, 1.f, 1.f, 1.f};
    constexpr Color Black   = {0.f, 0.f, 0.f, 1.f};
    constexpr Color Clear   = {0.f, 0.f, 0.f, 0.f};
    constexpr Color Red     = {1.f, 0.f, 0.f, 1.f};
    constexpr Color Green   = {0.f, 1.f, 0.f, 1.f};
    constexpr Color Blue    = {0.f, 0.f, 1.f, 1.f};
    constexpr Color Cyan    = {0.f, 1.f, 1.f, 1.f};
    constexpr Color Magenta = {1.f, 0.f, 1.f, 1.f};
    constexpr Color Yellow  = {1.f, 1.f, 0.f, 1.f};

} // namespace Colors


#endif // MATH_TYPES_H