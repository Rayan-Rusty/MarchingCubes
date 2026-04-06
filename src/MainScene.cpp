#include "MainScene.h"
#include "Utils.h"
#include "Tables.h"
#include "raymath.h"
#include <thread>
#include <iostream>

MainScene::MainScene()
    :m_Resolution{32}
{
    DisableCursor();

    auto start = std::chrono::high_resolution_clock::now();
    ThreadingInitCpu();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time Threading: " << elapsed.count() << " seconds\n";

    // start = std::chrono::high_resolution_clock::now();
    // InitCPU();
    // end = std::chrono::high_resolution_clock::now();
    // elapsed = end - start;
    //
    // std::cout << "Time Without Threading: " << elapsed.count() << " seconds\n";

}


void MainScene::InitGPU()
{

}

void MainScene::DrawGPU() const
{
;
}

MainScene::~MainScene()
{

}


void MainScene::Run(float deltaTime)
{

    Update(deltaTime);
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);
    BeginMode3D(worldCamera);
    Draw();
    EndMode3D();

    EndDrawing();
}


void MainScene::Draw() const
{
    DrawCPU();
}



void MainScene::Update(float deltaTime)
{
    UpdateCamera(&worldCamera , CAMERA_FREE);
}


void MainScene::initNoise()
{

}

void MainScene::ThreadingInitCpu()
{

    worldCamera.position = Vector3{ -3.f , 4.f , 10.f };
    worldCamera.target = Vector3{0.0f,0.0f,0.0f };
    worldCamera.up = Vector3{ 0.f , 1.f, 0.f };
    worldCamera.fovy = 45.f;
    worldCamera.projection = CAMERA_PERSPECTIVE;


    unsigned int nThreads {std::thread::hardware_concurrency()};
    int chunkSize  = m_Resolution / nThreads ;


    int half = m_Resolution / 2;
    voxelGrid.resize(m_Resolution * m_Resolution * m_Resolution);



    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < nThreads; i++)
        {
            int zStart{i * chunkSize};
            int zEnd{zStart + chunkSize};

            threads.emplace_back(Utils::LoadChunks,zStart , zEnd ,m_Resolution,std::ref(voxelGrid));
        }
    }





}


void MainScene::InitCPU()
{

    worldCamera.position = Vector3{ -3.f , 4.f , 10.f };
    worldCamera.target = Vector3{0.0f,0.0f,0.0f };
    worldCamera.up = Vector3{ 0.f , 1.f, 0.f };
    worldCamera.fovy = 45.f;
    worldCamera.projection = CAMERA_PERSPECTIVE;





    int half = m_Resolution / 2;
    voxelGrid.reserve(m_Resolution * m_Resolution * m_Resolution);



    for (int z = 0; z < m_Resolution; z++)
        for (int y = 0; y < m_Resolution; y++)
            for (int x = 0; x < m_Resolution; x++)
            {
                int worldX{ x - half};
                int worldY{y - half};
                int worldZ{ z - half };

                float scalar = Utils::Density(float(worldX), float(worldY), float(worldZ));
                Color col = PURPLE;

                if (scalar <= 0) col = RED;
                voxelGrid.push_back({ scalar, col });
            }

}

void MainScene::DrawCPU() const
{




    DrawGrid(m_Resolution, 1.0f);

    int size{ m_Resolution };
    int index{ 0 };
    int half{ m_Resolution / 2 };
    constexpr float scale{ 1.f };





    for (int z = 0; z < m_Resolution - 1; z++)
    for (int y = 0; y < m_Resolution - 1; y++)
    for (int x = 0; x < m_Resolution - 1; x++)
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
    for (int z = 0; z < m_Resolution; z++)
    for (int y = 0; y < m_Resolution; y++)
    for (int x = 0; x < m_Resolution; x++)
    {
    //
       index++;
    }
    //
    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
    DrawCubeWires(cubePosition, m_Resolution, m_Resolution, m_Resolution, PURPLE);

}

