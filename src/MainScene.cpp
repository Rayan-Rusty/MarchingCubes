#include "MainScene.h"
#include "Utils.h"
#include "Tables.h"
#include "raymath.h"
#include <thread>
#include <iostream>

MainScene::MainScene()
    :m_Resolution{64}
{
    DisableCursor();




    for (int width = 0; width < 2;width++)
    for (int depth = 0; depth < 2; depth++)
    for (int height = 0; height < 2; height++)
    {
        m_Positions.push_back(Vector3{float(width) ,float(height) ,float(depth) });
    }




    auto start = std::chrono::high_resolution_clock::now();
    ThreadingInitCpu();
    GenerateMesh();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time Threading: " << elapsed.count() << " seconds\n";


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


void MainScene::Draw()
{
    RenderMesh();
}


void MainScene::GenerateMesh()
{
    m_Triangles.clear();
    m_Triangles.resize(m_Chunks.size());
    std::vector<std::jthread> threads;
    //for future threading
     threads.reserve(m_Chunks.size());

     for (size_t i = 0; i < m_Chunks.size(); ++i)
     {
         threads.emplace_back([this, i]()
         {
             Utils::DrawChunks(m_Resolution , m_Positions[i] , m_Chunks[i] , m_Triangles[i]);
         });
     }

     threads.clear();


    // for (size_t i = 0; i < m_Chunks.size(); ++i)
    // {
    //      Utils::DrawChunks(m_Resolution, m_Positions[i], m_Chunks[i] , m_Triangles[i]);
    // }

}


void MainScene::RenderMesh() const
{
    for (const auto& mesh : m_Triangles)
    {
        for (const auto& tri : mesh)
        {
            DrawLine3D(tri.v0, tri.v1, BLACK);
            DrawLine3D(tri.v1, tri.v2, BLACK);
            DrawLine3D(tri.v2, tri.v0, BLACK);
            DrawTriangle3D(tri.v0, tri.v2, tri.v1, PURPLE);
        }
    }

    for (int i = 0; i < m_Chunks.size(); i++)
    {
        Vector3 cubePosition{m_Positions[i].x * m_Resolution, m_Positions[i].y* m_Resolution, m_Positions[i].z * m_Resolution};
        DrawCubeWires(cubePosition, m_Resolution, m_Resolution, m_Resolution, GREEN);

    }

}


void MainScene::Update(float deltaTime)
{
    UpdateCamera(&worldCamera , CAMERA_FREE);
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


    voxelGrid.resize(m_Resolution * m_Resolution * m_Resolution);

    m_Chunks.resize(m_Positions.size());

    {

        for (int c = 0; c < m_Positions.size(); c++)
        {
            m_Chunks[c].resize(m_Resolution * m_Resolution * m_Resolution);

            std::vector<std::jthread> threads;

            for (int i = 0; i < nThreads; i++)
            {
                int zStart{i * chunkSize};
                int zEnd{zStart + chunkSize};

                threads.emplace_back(Utils::LoadChunks,zStart , zEnd  ,m_Resolution, m_Positions[c] ,std::ref(m_Chunks[c]));
            }
        }
    }




}


void MainScene::InitCPU()
{

}

void MainScene::DrawCPU() const
{

}

