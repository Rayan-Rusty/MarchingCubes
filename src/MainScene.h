#ifndef MARCHINGCUBES_MAINSCENE_H
#define MARCHINGCUBES_MAINSCENE_H
#include "raylib.h"
#include <vector>
#include "DataTypes.h"
#include "FastNoiseLite.h"
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


	void GenerateMesh();
	void SetMeshDataIntoModels();
	void RenderMesh() const;


	void ThreadingInitCpu();


	//variables
	Camera3D worldCamera{ 0 };
	std::vector<std::vector<Voxel>> m_Chunks;
	std::vector<Voxel> voxelGrid;
	std::vector<std::vector<Triangle>> m_Triangles;
	std::vector<ChunkMeshData> m_MeshData;
	std::vector<Mesh> m_Meshes;
	std::vector<Model> m_Models;
	std::vector<Vector3> m_Positions;

	FastNoiseLite noise;
	FastNoiseLite m_NoiseMap;

	int m_Resolution;




};
#endif //MARCHINGCUBES_MAINSCENE_H