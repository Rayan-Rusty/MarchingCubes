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


	//variables
	Camera3D worldCamera{ 0 };
	std::vector<Voxel> voxelGrid;

	int m_Resolution;
};