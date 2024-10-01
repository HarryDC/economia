#include "raymath.h"


inline constexpr Vector3& operator+=(Vector3& left, const Vector3& right) noexcept {
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;
    return left;
}

inline constexpr Vector3& operator+=(Vector3& left, const float right) noexcept {
    left.x += right;
    left.y += right;
    left.z += right;
    return left;
}

inline constexpr Vector3 operator+(Vector3 left, const Vector3& right) noexcept {
    return left += right;
}

inline constexpr Vector3 operator+(Vector3 left, const float right) noexcept {
    return left += right;
}

inline constexpr Vector3 operator+(const float left, Vector3 right) noexcept {
    return right += left;
}

inline constexpr Vector3& operator-=(Vector3& left, const Vector3& right) noexcept {
    left.x -= right.x;
    left.y -= right.y;
    left.z -= right.z;
    return left;
}

inline constexpr Vector3& operator-=(Vector3& left, const float right) noexcept {
    left.x -= right;
    left.y -= right;
    left.z -= right;
    return left;
}

inline constexpr Vector3 operator-(Vector3 left, const Vector3& right) noexcept {
    return left -= right;
}

inline constexpr Vector3 operator-(Vector3 left, const float right) noexcept  {
    return left -= right;
}

inline constexpr Vector3& operator*=(Vector3& left, const float right) noexcept {
    left.x *= right;
    left.y *= right;
    left.z *= right;
    return left;
}

inline constexpr Vector3 operator*(Vector3 left, const float right) noexcept {
    return left *= right;
}

inline constexpr Vector3 operator*(const float left, Vector3 right) noexcept {
    return right *= left;
}
