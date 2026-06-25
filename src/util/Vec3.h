#pragma once

struct Vec3 {
    float x = 0;
    float y = 0;
    float z = 0;

    static const Vec3 ZERO;
    static const Vec3 ONE;
    static const Vec3 UP;
    static const Vec3 DOWN;
    static const Vec3 FORWARD;
    static const Vec3 BACK;
};

inline const Vec3 Vec3::ZERO = {0, 0, 0};
inline const Vec3 Vec3::ONE = {1, 1, 1};
inline const Vec3 Vec3::UP = {0, 1, 0};
inline const Vec3 Vec3::DOWN = {0, -1, 0};
inline const Vec3 Vec3::FORWARD = {0, 0, -1};
inline const Vec3 Vec3::BACK = {0, 0, 1};
