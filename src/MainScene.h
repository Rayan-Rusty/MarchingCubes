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
	void Draw() const;


	void initNoise();


	void InitCPU();
	void ThreadingInitCpu();

	void DrawCPU() const;

	//variables
	Camera3D worldCamera{ 0 };
	std::vector<Voxel> voxelGrid;
	std::vector<Voxel> voxelGrid2;

	int m_Resolution;


	Shader m_Shader;
	Mesh m_cubeMesh;
	Model m_Model;


};