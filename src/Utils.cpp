#include "Utils.h"
#include "raymath.h"
#include "Tables.h"
#include "FastNoiseLite.h"
float Utils::Density(float x, float y, float z)
{

    // float h = 4.0f;
    //
    // constexpr float half = 10.f;

    constexpr float groundLevel{ 5.f };

    constexpr float radius{ 5.f };
    float scalar{ y - groundLevel };
    float noise = noiseMap.GetNoise(x, y, z);
    scalar -= noise * 2.0f;

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


void Utils::LoadChunks(int zStart, int zEnd , int total, const Vector3& ChunkPos , std::vector<Voxel>& voxelGrid)
{
    int half = total / 2;
    for (int z = zStart; z < zEnd; z++)
    for (int y = 0; y < total; y++)
    for (int x = 0; x < total; x++)
    {
        int worldX = x + ChunkPos.x * (total - 1) - half;
        int worldY = y + ChunkPos.y * (total - 1)- half;
        int worldZ = z + ChunkPos.z * (total - 1)- half;

        float scalar = Utils::Density(
            static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ));

        int index = x
                  + y * total
                  + z * total * total;

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
    int index{ 0 };
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
            cornersCell[i] =
                Vector3
                {
                    ((x + corners[i].x) + ChunkPos.x * (resolution - 1) - half) * scale,
                    ((y + corners[i].y) + ChunkPos.y * (resolution - 1)- half) * scale,
                    ((z + corners[i].z) + ChunkPos.z * (resolution - 1) - half) * scale
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
            //outMesh.push_back({v0, v1 , v2});


            Vector3 n =
                Vector3Normalize(
                  Vector3CrossProduct(
                    Vector3Subtract(v1,v0),
                    Vector3Subtract(v2,v0)
                  ));

            Vector3 tri[3] = { v0, v2, v1 };

            for(int k=0;k<3;k++)
            {
                verts.push_back(tri[k].x);
                verts.push_back(tri[k].y);
                verts.push_back(tri[k].z);

                normals.push_back(n.x);
                normals.push_back(n.y);
                normals.push_back(n.z);
            }

        }

    }



    outMesh.verts = std::move(verts);
    outMesh.normals = std::move(normals);



    index++;
}
