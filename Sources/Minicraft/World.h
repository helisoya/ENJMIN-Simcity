#pragma once

#include "Engine/BlendState.h"
#include "Engine/Camera.h"
#include "Minicraft/Block.h"
#include "Minicraft/Chunk.h"
#include "Minicraft/Cube3D.h"

#define WORLD_SIZE 6
#define WORLD_HEIGHT 1

class Chunk;
class Cube3D;

enum Building {
	NOTHING,

	TREE,

	HOUSE,
	SHOP,
	FACTORY,

	WATERPLANT,
	ENERGYPLANT,

	ROAD
};

struct BuildingData {
	Cube3D* model;
	std::vector<Vector3>* positions;
};

class World {
	Chunk* chunks[WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE];
	Building buildings[WORLD_SIZE * 16 * WORLD_SIZE * 16];
	std::map<Building, BuildingData> buildingsPositions;

public:
	World();
	virtual ~World();
	void Generate(DeviceResources* deviceRes);
	void GenerateFromFile(DeviceResources* deviceRes, std::wstring filePath);
	void Draw(Camera* camera, DeviceResources* deviceRes);

	Chunk* GetChunk(int cx, int cy, int cz);
	Chunk* GetChunkFromCoordinates(int gx, int gy, int gz);
	BlockId* GetCube(int gx, int gy, int gz);
	void MakeChunkDirty(int gx, int gy, int gz);

	void UpdateBlock(int gx, int gy, int gz, BlockId block);

	Building GetBuilding(int x,int y);
	void PlaceBuilding(Building type, int x, int y, int z);

	friend class Chunk;

private:
	void Create(DeviceResources* deviceRes);
};
