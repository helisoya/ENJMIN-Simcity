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
	int energy;
	int water;
	int income;
};

class World {
	Chunk* chunks[WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE];
	Building buildings[WORLD_SIZE * 16 * WORLD_SIZE * 16];
	std::map<Building, BuildingData> buildingsPositions;

	int energyGain = 0;
	int waterGain = 0;
	int passiveIncome = 1;

	DeviceResources* deviceRes;

public:
	World();
	virtual ~World();
	void Generate(DeviceResources* deviceRes, int seed, float treeThreshold);
	void GenerateFromFile(DeviceResources* deviceRes, std::wstring filePath, float treeThreshold);
	void Draw(Camera* camera, DeviceResources* deviceRes);
	void DrawBuildings(Camera* camera, DeviceResources* deviceRes);
	void Reset();

	Chunk* GetChunk(int cx, int cy, int cz);
	Chunk* GetChunkFromCoordinates(int gx, int gy, int gz);
	BlockId* GetCube(int gx, int gy, int gz);
	void MakeChunkDirty(int gx, int gy, int gz);
	bool IsAdjacentToWater(int gx, int gy, int gz);

	void UpdateBlock(int gx, int gy, int gz, BlockId block);

	Building GetBuilding(int x,int y);
	void PlaceBuilding(Building type, int x, int y, int z);
	void RemoveBuilding(int x, int y, int z);
	int GetWaterDelta();
	int GetEnergyDelta();
	int GetPassiveIncome();

	int GetAmountOfAdjacentRoads(int x,int y);

	friend class Chunk;

private:
	void RegenerateBufferFor(Building building);
	void Create(DeviceResources* deviceRes);
};
