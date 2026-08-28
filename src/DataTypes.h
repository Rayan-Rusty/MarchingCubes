
#ifndef MARCHINGCUBES_DATATYPES_H
#define MARCHINGCUBES_DATATYPES_H
#include "raylib.h"
#include <vector>

struct Voxel
{
    float density;
    Color color;
};


struct Triangle
{
    Vector3 v0;
    Vector3 v1;
    Vector3 v2;
};

struct ChunkMeshData
{
    std::vector<float> verts;
    std::vector<float> normals;
};
#endif //MARCHINGCUBES_DATATYPES_H