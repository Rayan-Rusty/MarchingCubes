#include "MainScene.h"
#include "Utils.h"
#include <thread>
#include <iostream>

#include "raymath.h"

MainScene::MainScene()
    :m_Resolution{16}
{
    DisableCursor();

    Utils::noiseMap.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    Utils::noiseMap.SetFrequency(0.15f);
    Utils::noiseMap.SetFractalOctaves(3);
    Utils::noiseMap.SetFractalType(FastNoiseLite::FractalType_FBm);


    constexpr int amnt{4};
    for (int width = 0; width <amnt;width++)
    for (int depth = 0; depth < amnt; depth++)
    for (int height = 0; height < amnt; height++)
    {
        m_Positions.push_back(Vector3{static_cast<float>(width) ,static_cast<float>(height) ,static_cast<float>(depth) });
    }




    auto start = std::chrono::high_resolution_clock::now();

    ThreadingInitCpu();
    GenerateMesh();
    SetMeshDataIntoModels();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time Threading: " << elapsed.count() << " seconds\n";


}


MainScene::~MainScene()
{
    for (auto& model : m_Models)
        UnloadModel(model);
}


void MainScene::Run(float deltaTime)
{

    Update(deltaTime);
    BeginDrawing();
    ClearBackground(Color{0, 0, 150});
    DrawFPS(10, 10);
    BeginMode3D(worldCamera);
    Draw();
    EndMode3D();

    EndDrawing();
}


void MainScene::Draw() const
{
    RenderMesh();
}


void MainScene::GenerateMesh()
{
    m_Triangles.clear();
    m_Triangles.resize(m_Chunks.size());
    m_Meshes.resize(m_Chunks.size());
    m_MeshData.resize(m_Chunks.size());

    std::vector<std::jthread> threads;
    threads.reserve(m_Chunks.size());

        for (size_t i = 0; i < m_Chunks.size(); ++i)
        {
            threads.emplace_back([this, i]()
            {
                Utils::DrawChunks(m_Resolution , m_Positions[i] , m_Chunks[i] , std::ref(m_MeshData[i]));
            });
        }

    threads.clear();

}


void MainScene::SetMeshDataIntoModels()
{
    m_Models.resize(m_MeshData.size());
    for (size_t i = 0; i < m_MeshData.size(); i++)
    {
        Mesh mesh = {0};
        auto& data = m_MeshData[i];
        mesh.vertexCount = static_cast<int>(data.verts.size() / 3);
        mesh.triangleCount = mesh.vertexCount / 3; // still assumes non-indexed

        size_t vertBytes = data.verts.size() * sizeof(float);
        mesh.vertices = static_cast<float*>(MemAlloc(vertBytes));
        if (!mesh.vertices)
        {
            /* handle OOM */ continue;
        }
        memcpy(mesh.vertices, data.verts.data(), vertBytes);

        size_t normBytes = data.normals.size() * sizeof(float);
        mesh.normals = static_cast<float*>(MemAlloc(normBytes));
        if (!mesh.normals)
        {
            /* handle OOM */ continue;
        }
        memcpy(mesh.normals, data.normals.data(), normBytes);


        if (data.normals.size() != data.verts.size())
        {
            std::cerr << "Mismatched vert/normal count for chunk " << i << "\n";
        }

        UploadMesh(&mesh, false);
        m_Models[i] = LoadModelFromMesh(mesh);
    }
}


void MainScene::RenderMesh() const
{
    for (const auto& model : m_Models)
    {
        DrawModel(model, Vector3Zero(), 1.0f, GREEN);
        DrawModelWires(model, Vector3Zero(), 1.0f, BLACK);
    }

    auto res = static_cast<float>(m_Resolution);
    for (int i = 0; i < m_Models.size(); i++)
    {
        Vector3 cubePosition{m_Positions[i].x * res, m_Positions[i].y* res, m_Positions[i].z * res};
        DrawCubeWires(cubePosition, res, res, res, WHITE);
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


