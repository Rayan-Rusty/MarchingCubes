#pragma once
#include "raylib.h"
#include <vector>
#include "DataTypes.h"
namespace Utils
{
	float Density(float x, float y, float z);

	float IndexD(float x, float y, float z, int size, const std::vector<Voxel>& voxelGrid);

	float IndexD(Vector3 xyz , float size, const std::vector<Voxel>& voxelGrid);

	void LoadChunks(int zStart , int zEnd , int total, std::vector<Voxel>& voxelGrid);


	void DrawChunks(int resolution , std::vector<Voxel>& voxelGrid);
}