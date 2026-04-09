#pragma once
#include <memory>
#include "raylib.h"
#include <vector>

struct Voxel
{
    float density;
    Color color;
};


struct Triangle
{
public:
    Vector3 v0;
    Vector3 v1;
    Vector3 v2;
};