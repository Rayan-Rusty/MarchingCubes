#include "Utils.h"
#include "raymath.h"
#include "Tables.h"
#include "FastNoiseLite.h"
float Utils::Density(float x, float y, float z)
{
    constexpr float groundLevel{ 5.f };

    float scalar{ y - groundLevel };
    float noise = noiseMap.GetNoise(x, y, z);

    scalar -= noise * 2.0f;
    return scalar;
}

float Utils::IndexD(int x, int y, int z, int size, const std::vector<Voxel>& voxelGrid)
{

    const int index = x + y * size + z * size * size;
    return voxelGrid[index].density;
}

float Utils::IndexD(Vector3 xyz, float size, const std::vector<Voxel>& voxelGrid)
{
    return IndexD(
        static_cast<int>(xyz.x),
        static_cast<int>(xyz.y),
        static_cast<int>(xyz.z),
        static_cast<int>(size),
        voxelGrid);
}


void Utils::LoadChunks(int zStart, int zEnd , int total, const Vector3& ChunkPos , std::vector<Voxel>& voxelGrid)
{
    int half { total / 2 };
    for (int z = zStart; z < zEnd; z++)
    for (int y = 0; y < total; y++)
    for (int x = 0; x < total; x++)
    {
        float worldX = static_cast<float>(x) + ChunkPos.x * static_cast<float>(total - 1) - static_cast<float>(half);
        float worldY = static_cast<float>(y) + ChunkPos.y * static_cast<float>(total - 1) - static_cast<float>(half);
        float worldZ = static_cast<float>(z) + ChunkPos.z * static_cast<float>(total - 1) - static_cast<float>(half);


        float scalar { Density(worldX,worldY,worldZ)};

        int index { x
                  + y * total
                  + z * total * total };

        voxelGrid[index] = { scalar, PURPLE };
    }
}


int Utils::CubeIndex(float *ds[])
{
    int cubeIndex{ 0 };
    for (size_t i = 0; i < 8; i++)
        if (*ds[i] < 0.f)
            cubeIndex |= (1 << i); // bitshifting

    return cubeIndex;

}

void Utils::DrawChunks(int resolution, const Vector3& ChunkPos, const std::vector<Voxel> &voxelGrid,  ChunkMeshData&  outMesh)
{
    int size{ resolution };
    int half{ resolution / 2 };


    std::vector<float> verts;
    std::vector<float> normals;


    for (int z = 0; z < resolution - 1; z++)
    for (int y = 0; y < resolution - 1; y++)
    for (int x = 0; x < resolution - 1; x++)
    {
        //8 sides in the Cell
        Vector3 cornersCell[8];
        for (int i = 0; i < 8; ++i)
        {
            constexpr float scale{ 1.f };
            cornersCell[i] =  Vector3
                {
                    (static_cast<float>(x) + corners[i].x + ChunkPos.x * static_cast<float>(resolution - 1) - static_cast<float>(half)) * scale,
                    (static_cast<float>(y) + corners[i].y + ChunkPos.y * static_cast<float>(resolution - 1) - static_cast<float>(half)) * scale,
                    (static_cast<float>(z) + corners[i].z + ChunkPos.z * static_cast<float>(resolution - 1) - static_cast<float>(half)) * scale
                };
        }

        float d0 = IndexD(x, y, z, size, voxelGrid);
        float d1 = IndexD(x + 1, y, z, size, voxelGrid);
        float d2 = IndexD(x + 1, y + 1, z, size, voxelGrid);
        float d3 = IndexD(x, y + 1, z, size, voxelGrid);

        float d4 = IndexD(x, y, z + 1, size, voxelGrid);
        float d5 = IndexD(x + 1, y, z + 1, size, voxelGrid);
        float d6 = IndexD(x + 1, y + 1, z + 1, size, voxelGrid);
        float d7 = IndexD(x, y + 1, z + 1, size, voxelGrid);

        float* ds[8] = { &d0,&d1,&d2, &d3, &d4, &d5, &d6, &d7 };
        int cubeIndex{CubeIndex(ds)};
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

            Vector3 n =
                Vector3Normalize(
                  Vector3CrossProduct(
                    Vector3Subtract(v1,v0),
                    Vector3Subtract(v2,v0)
                  ));

            Vector3 tri[3] = { v0, v2, v1 };

            for (const auto& v : tri)
            {
                verts.insert(verts.end(), { v.x, v.y, v.z });
                normals.insert(normals.end(), { n.x, n.y, n.z });
            }
        }
    }
    outMesh.verts = std::move(verts);
    outMesh.normals = std::move(normals);
}