#include "Utils.h"
#include "raymath.h"
#include "Tables.h"
#include "FastNoiseLite.h"
float Utils::Density(float x, float y, float z)
{

    float h = 4.0f;

    constexpr float half = 10.f;

    constexpr float radius{ 5.f };
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

void Utils::DrawChunks(int resolution, std::vector<Voxel> &voxelGrid)
{




    DrawGrid(resolution, 1.0f);

    int size{ resolution };
    int index{ 0 };
    int half{ resolution / 2 };
    constexpr float scale{ 1.f };





    for (int z = 0; z < resolution - 1; z++)
    for (int y = 0; y < resolution - 1; y++)
    for (int x = 0; x < resolution - 1; x++)
    {




        //8 sides in the Cell
        Vector3 cornersCell[8];
        for (int i = 0; i < 8; ++i)
        {
            cornersCell[i] =
                Vector3
                {
                    (x + corners[i].x - half) * scale,
                    (y + corners[i].y - half) * scale,
                    (z + corners[i].z - half) * scale
                };
        }

        float d0 = Utils::IndexD(x, y, z, size, voxelGrid);
        float d1 = Utils::IndexD(x + 1, y, z, size, voxelGrid);
        float d2 = Utils::IndexD(x + 1, y + 1, z, size, voxelGrid);
        float d3 = Utils::IndexD(x, y + 1, z, size, voxelGrid);

        float d4 = Utils::IndexD(x, y, z + 1, size, voxelGrid);
        float d5 = Utils::IndexD(x + 1, y, z + 1, size, voxelGrid);
        float d6 = Utils::IndexD(x + 1, y + 1, z + 1, size, voxelGrid);
        float d7 = Utils::IndexD(x, y + 1, z + 1, size, voxelGrid);

        float* ds[8] = { &d0,&d1,&d2, &d3, &d4, &d5, &d6, &d7 };
        int cubeIndex{ 0 };
        for (size_t i = 0; i < 8; i++)
            if (*ds[i] < 0.f)
                cubeIndex |= (1 << i); // bitshifting

        int edges = edgeTable[cubeIndex];



        if (edges == 0) continue;


        //interpolation
        Vector3 vertexList[12]{};



        for (size_t i = 0; i < 12; i++)
        {
            if (edges & (1 << i))
            {
                int a{ edgeToCorner[i][0] };
                int b{ edgeToCorner[i][1] };

                float valA = *ds[a];
                float valB = *ds[b];

                float t{ (0.f - valA) / (valB - valA) };
                vertexList[i] = Vector3Lerp(cornersCell[a] , cornersCell[b] , t);
            }

        }


        for (int i = 0; triTable[cubeIndex][i] != -1; i += 3)
        {
            Vector3 v0 = vertexList[triTable[cubeIndex][i]];
            Vector3 v1 = vertexList[triTable[cubeIndex][i + 1]];
            Vector3 v2 = vertexList[triTable[cubeIndex][i + 2]];


            DrawLine3D(v0, v1, BLACK);
            DrawLine3D(v1, v2, BLACK);
            DrawLine3D(v2, v0, BLACK);
            DrawTriangle3D(v0, v2, v1, GREEN);
        }


        index++;
    }

    //
    // // DEBUG: draw voxelGrid as tiny cubes
    index = 0;
    //
    for (int z = 0; z < resolution; z++)
    for (int y = 0; y < resolution; y++)
    for (int x = 0; x < resolution; x++)
    {
    //
       index++;
    }
    //
    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
    DrawCubeWires(cubePosition, resolution, resolution, resolution, PURPLE);

}
