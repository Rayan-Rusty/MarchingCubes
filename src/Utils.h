#pragma once
#include "raylib.h"
#include <vector>
#include "DataTypes.h"
namespace Utils
{
	float Density(float x, float y, float z);

	float IndexD(float x, float y, float z, int size, const std::vector<Voxel>& voxelGrid);

	float IndexD(Vector3 xyz , float size, const std::vector<Voxel>& voxelGrid);

	void LoadChunks(int zStart , int zEnd , int total,const Vector3& ChunkPos, std::vector<Voxel>& voxelGrid);
	int CubeIndex(float *ds[]);

	void DrawChunks(int resolution , const Vector3& ChunkPos ,const std::vector<Voxel>& voxelGrid , std::vector<Triangle>& outTriangles);
}