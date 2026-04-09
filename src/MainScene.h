#pragma once
#include <memory>
#include "raylib.h"
#include <vector>
#include "DataTypes.h"
class MainScene final
{
public:
	MainScene();
	~MainScene();

	void Run(float deltaTime);
private:

	//functions
	void Update(float deltaTime);
	void Draw();


	void GenerateMesh();
	void RenderMesh() const;



	void InitCPU();
	void ThreadingInitCpu();

	void DrawCPU() const;

	//variables
	Camera3D worldCamera{ 0 };
	std::vector<std::vector<Voxel>> m_Chunks;
	std::vector<Voxel> voxelGrid;
	std::vector<std::vector<Triangle>> m_Triangles;
	std::vector<Vector3> m_Positions;

	int m_Resolution;



	bool m_Initialized{false};

};