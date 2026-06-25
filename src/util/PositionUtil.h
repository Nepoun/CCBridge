#pragma once
#include "raymath.h"
#include "Vec3.h"

Vector3 toRaylib(Vec3 v)
{
    return {v.x, v.y, v.z};
}