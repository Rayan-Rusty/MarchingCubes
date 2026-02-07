#pragma once
#include "raylib.h"
#include <vector>
#include "DataTypes.h"
namespace Utils
{
	float Density(float x, float y, float z);

	float IndexD(float x, float y, float z, int size, const std::vector<Voxel>& voxelGrid);

	float IndexD(Vector3 xyz , float size, const std::vector<Voxel>& voxelGrid);

	float DensityRandom(float x, float y, float z);
	float Noise(float x, float z);
}