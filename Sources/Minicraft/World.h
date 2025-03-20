#pragma once

#include "Engine/BlendState.h"
#include "Engine/Camera.h"
#include "Minicraft/Block.h"

#define WORLD_SIZE 6
#define WORLD_HEIGHT 1

class Chunk;
class Cube3D;

/// <summary>
/// Represents the different building in the game
/// </summary>
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

/// <summary>
/// Represents a building's data
/// </summary>
struct BuildingData {
	Cube3D* model;
	std::vector<Vector3>* positions;
	int energy;
	int water;
	int income;
};

/// <summary>
///  Represents the world
/// </summary>
class World {
	Chunk* chunks[WORLD_SIZE * WORLD_HEIGHT * WORLD_SIZE];
	Building buildings[WORLD_SIZE * 16 * WORLD_SIZE * 16];
	std::map<Building, BuildingData> buildingsPositions;

	int energyGain = 0;
	int waterGain = 0;
	int passiveIncome = 0;

	DeviceResources* deviceRes;

public:
	World();
	virtual ~World();

	/// <summary>
	/// Generates the world using procedural generation
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="seed">The map's seed</param>
	/// <param name="treeThreshold">The map's tree threshold</param>
	void Generate(DeviceResources* deviceRes, int seed, float treeThreshold);

	/// <summary>
	/// Generates the world using a premade file
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="filePath">The map's filepath</param>
	/// <param name="treeThreshold">The map's tree threshold</param>
	void GenerateFromFile(DeviceResources* deviceRes, std::wstring filePath, float treeThreshold);

	/// <summary>
	/// Draws the world
	/// </summary>
	/// <param name="camera">The game's camera</param>
	/// <param name="deviceRes">The game's device resources</param>
	void Draw(Camera* camera, DeviceResources* deviceRes);


	/// <summary>
	/// Draws the world's buildings
	/// </summary>
	/// <param name="camera">The game's camera</param>
	/// <param name="deviceRes">The game's device resources</param>
	void DrawBuildings(Camera* camera, DeviceResources* deviceRes);

	// Reset the world
	void Reset();

	/// <summary>
	/// Gets a chunk
	/// </summary>
	/// <param name="cx">The chunk's X position</param>
	/// <param name="cy">The chunk's Y position</param>
	/// <param name="cz">The chunk's Z position</param>
	/// <returns>The chunk</returns>
	Chunk* GetChunk(int cx, int cy, int cz);

	/// <summary>
	/// Gets a chunk from a global coordinate
	/// </summary>
	/// <param name="cx">The coordinate's X position</param>
	/// <param name="cy">The coordinate's Y position</param>
	/// <param name="cz">The coordinate's Z position</param>
	/// <returns>The chunk</returns>
	Chunk* GetChunkFromCoordinates(int gx, int gy, int gz);

	/// <summary>
	/// Gets a cube from a global coordinate
	/// </summary>
	/// <param name="cx">The coordinate's X position</param>
	/// <param name="cy">The coordinate's Y position</param>
	/// <param name="cz">The coordinate's Z position</param>
	/// <returns>The cube</returns>
	BlockId* GetCube(int gx, int gy, int gz);

	/// <summary>
	/// Make a chunk dirty
	/// </summary>
	/// <param name="cx">The chunk's X position</param>
	/// <param name="cy">The chunk's Y position</param>
	/// <param name="cz">The chunk's Z position</param>
	void MakeChunkDirty(int gx, int gy, int gz);

	/// <summary>
	/// Checks if a coordinate is adjacent to water
	/// </summary>
	/// <param name="cx">The coordinate's X position</param>
	/// <param name="cy">The coordinate's Y position</param>
	/// <param name="cz">The coordinate's Z position</param>
	/// <returns>True if it is adjacent to water</returns>
	bool IsAdjacentToWater(int gx, int gy, int gz);

	/// <summary>
	/// Updates a block's ID
	/// </summary>
	/// <param name="cx">The block's X position</param>
	/// <param name="cy">The block's Y position</param>
	/// <param name="cz">The block's Z position</param>
	/// <param name="block">The new block's ID</param>
	void UpdateBlock(int gx, int gy, int gz, BlockId block);

	/// <summary>
	/// Gets a building on the map
	/// </summary>
	/// <param name="x">The building's X position</param>
	/// <param name="y">The building's Y position</param>
	/// <returns>The building</returns>
	Building GetBuilding(int x,int y);

	/// <summary>
	/// Place a building on the map
	/// </summary>
	/// <param name="type">the building's type</param>
	/// <param name="x">The building's X position</param>
	/// <param name="y">The building's Y position</param>
	/// <param name="z">The building's Z position</param>
	void PlaceBuilding(Building type, int x, int y, int z);

	/// Removes a building
	/// </summary>
	/// <param name="x">The building's X position</param>
	/// <param name="y">The building's Y position</param>
	/// <param name="z">The building's Z position</param>
	void RemoveBuilding(int x, int y, int z);

	// Gets the delta for the water consumption
	int GetWaterDelta();
	// Gets the delta for the energy consumption
	int GetEnergyDelta();
	// Gets the city's passive income
	int GetPassiveIncome();

	/// <summary>
	/// Gets the number of adjacent road for a building
	/// </summary>
	/// <param name="x">The building's X position</param>
	/// <param name="y">The building's Y position</param>
	/// <returns>The number of adjacent roads</returns>
	int GetAmountOfAdjacentRoads(int x,int y);

	friend class Chunk;

private:
	/// <summary>
	/// Regenerates the buffer for a specific building type
	/// </summary>
	/// <param name="building">The building type</param>
	void RegenerateBufferFor(Building building);

	/// <summary>
	/// Creates the world
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes);
};
