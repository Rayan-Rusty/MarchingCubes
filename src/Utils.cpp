#include "Utils.h"
#include "raymath.h"
#include "FastNoiseLite.h"
float Utils::Density(float x, float y, float z)
{

    float h = 4.0f;

    constexpr float half = 10.f;

    constexpr float radius{ 14.f };
    float scalar{ x * x + y * y + z * z - radius * radius };
    return scalar;

    //
    // float side = fabs(x) + fabs(z) + y - h;  // slanted sides
    // float bottom = -y - h;                  // bottom plane
    // return fmax(side, bottom);
}

float Utils::IndexD(float x, float y, float z, int size, const std::vector<Voxel>& voxelGrid)
{
    
    int ix = static_cast<int>(x);
    int iy = static_cast<int>(y);
    int iz = static_cast<int>(z);

    int index = ix + iy * size + iz * size * size;
    return voxelGrid[index].density;
}

float Utils::IndexD(Vector3 xyz, float size, const std::vector<Voxel>& voxelGrid)
{
	return IndexD(xyz.x, xyz.y, xyz.z, size, voxelGrid);
}


void Utils::LoadChunks(int zStart, int zEnd , int total , std::vector<Voxel>& voxelGrid)
{
    int half = total / 2;
    for (int z = zStart; z < zEnd; z++)
    for (int y = 0; y < total; y++)
    for (int x = 0; x < total; x++)
    {
        int worldX = x - half;
        int worldY = y - half;
        int worldZ = z - half;

        float scalar = Utils::Density(
            float(worldX), float(worldY), float(worldZ));

        int index = x
                  + y * total
                  + z * total * total;

        voxelGrid[index] = { scalar, PURPLE };
    }
}
