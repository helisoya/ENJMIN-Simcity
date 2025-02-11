#pragma once

#include "TP/Block.h"
#include "TP/Buffer.h"
#include "TP/Chunck.h"
#include "TP/Skybox.h"
#include "PerlinNoise.hpp"


class Camera;
#define MAP_SIZE 6

using namespace DirectX;
using namespace DirectX::SimpleMath;


class World {

	struct ModelData {
		Matrix model;
	};

public:

private:
	std::vector<Chunck> chuncks;
	Skybox skybox;

	ConstantBuffer<ModelData> constantBufferModel;

	DeviceResources* deviceRes;

public:

	World();

	void Generate(DeviceResources* deviceRes);
	void GenerateFromFile(DeviceResources* deviceRes, std::wstring filePath);

	void Draw(DeviceResources* deviceRes, Camera& camera);
	void DrawSkybox(Vector3 pos, DeviceResources* deviceRes);
	void DrawWater(DeviceResources* deviceRes, Camera& camera);

	void SearchForBlock(Vector3 pos, Vector3 forward,BlockId replaceWith);

private:

	Vector2 To2D(int x);
	int To1D(int x, int y);

	void SetupEmptyChunks(DeviceResources* deviceRes);

	void GenerateChunck(Vector3 pos, siv::BasicPerlinNoise<float> &noise, DeviceResources* deviceRes);
	void GenerateMesh(DeviceResources* deviceRes);
};