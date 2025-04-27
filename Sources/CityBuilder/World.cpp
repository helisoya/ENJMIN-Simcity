#include "pch.h"

#include "Engine/DefaultResources.h"
#include "World.h"
#include "PerlinNoise.hpp"
#include "iostream"
#include "fstream"
#include "sstream"
#include "Chunk.h"
#include "Building3D.h"

World::World() {

	// Generate empty world
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				Chunk* c = new Chunk(this, Vector3(x, y, z) * CHUNK_SIZE);
				chunks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_HEIGHT] = c;
			}
		}
	}
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				auto chunk = GetChunk(x, y, z);
				if (x > 0) chunk->adjXNeg = GetChunk(x - 1, y, z);
				if (y > 0) chunk->adjYNeg = GetChunk(x, y - 1, z);
				if (z > 0) chunk->adjZNeg = GetChunk(x, y, z - 1);
				if (x < WORLD_SIZE - 1) chunk->adjXPos = GetChunk(x + 1, y, z);
				if (y < WORLD_SIZE - 1) chunk->adjYPos = GetChunk(x, y + 1, z);
				if (z < WORLD_SIZE - 1) chunk->adjZPos = GetChunk(x, y, z + 1);
			}
		}
	}

	for (int x = 0; x < WORLD_SIZE * CHUNK_SIZE * WORLD_SIZE * CHUNK_SIZE; x++) {
		buildings[x] = NOTHING;
	}

	// Generate building datas

	BuildingData data = {};
	data.model = new Building3D(TREE);
	data.positions = new std::vector<Vector3>();
	data.energy = 0; data.water = 0; data.income = 0;
	buildingsPositions[TREE] = data;

	data = {};
	data.model = new Building3D(HOUSE);
	data.positions = new std::vector<Vector3>();
	data.energy = -1; data.water = -1; data.income = 2;
	buildingsPositions[HOUSE] = data;

	data = {};
	data.model = new Building3D(SHOP);
	data.positions = new std::vector<Vector3>();
	data.energy = -2; data.water = -1; data.income = 4;
	buildingsPositions[SHOP] = data;

	data = {};
	data.model = new Building3D(FACTORY);
	data.positions = new std::vector<Vector3>();
	data.energy = -5; data.water = -2; data.income = 8;
	buildingsPositions[FACTORY] = data;

	data = {};
	data.model = new Building3D(WATERPLANT);
	data.positions = new std::vector<Vector3>();
	data.energy = -2; data.water = 10; data.income = 0;
	buildingsPositions[WATERPLANT] = data;

	data = {};
	data.model = new Building3D(ENERGYPLANT);
	data.positions = new std::vector<Vector3>();
	data.energy = 5; data.water = 0; data.income = 0;
	buildingsPositions[ENERGYPLANT] = data;

	data = {};
	data.model = new Building3D(ROAD);
	data.positions = new std::vector<Vector3>();
	data.energy = 0; data.water = 0; data.income = 0;
	buildingsPositions[ROAD] = data;
}

World::~World() {
	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
		delete chunks[idx];
		chunks[idx] = nullptr;
	}
}

void World::Generate(DeviceResources* deviceRes,int seed, float treeThreshold) {
	
	this->deviceRes = deviceRes;

	Reset();

	siv::BasicPerlinNoise<float> perlin(seed);
	float noiseValue;
	int yMax;

	float treeNoiseValue;

	float scale = WORLD_SIZE * CHUNK_SIZE / 2.5;


	for (int x = 0; x < CHUNK_SIZE * WORLD_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE * WORLD_SIZE; z++) {
			// Sample noise
			noiseValue = (perlin.noise2D(x / scale, z / scale) + 1) / 2;
			treeNoiseValue = (perlin.noise2D(x / scale * 2, z / scale * 2) + 1) / 2;
			yMax = (int)(noiseValue * 6);

			// 0 = Sand
			// 1 - 2 = Grass
			// 3 - 4 = Rocks
			// If y == 0, then there will be water at (x,1,z)

			if (yMax <= 1 ) {
				auto block = GetCube(x, 1, z);
				*block = WATER;

				block = GetCube(x, 0, z);
				*block = SAND;
				continue;
			}

			for (int y = 0; y < yMax && y < 7; y++) {
				auto block = GetCube(x, y, z);

				if (y == 0) {
					*block = SAND;
				}
				else {
					*block = y < 3 ? GRASS : STONE;
				}
			}

			if (treeNoiseValue <= treeThreshold && yMax <= 3) {
				// Place tree
				PlaceBuilding(TREE, x,yMax, z);
			}
		}
	}

	Create(deviceRes);
}

void World::GenerateFromFile(DeviceResources* deviceRes, std::wstring filePath, float treeThreshold)
{
	this->deviceRes = deviceRes;

	Reset();

	// The seed is generated from the filename
	int treeSeed = 0;
	for (int i = 0; i < filePath.size(); i++) {
		treeSeed += filePath.c_str()[i];
	}

	siv::BasicPerlinNoise<float> perlin(treeSeed);
	float treeNoiseValue;
	float scale = WORLD_SIZE * CHUNK_SIZE / 2.5;


	std::fstream fin;
	std::wstring file = std::wstring(L"Tilemap/") + filePath + L".csv";

	fin.open(file.c_str(), std::ios::in);
	std::string line, word;
	int x = 0;
	int y = 0;
	int xInChunck = 0;
	int yInChunck = 0;
	int yMax;
	int value;
	BlockData dataSand = BlockData::Get(SAND);
	BlockData dataGrass = BlockData::Get(GRASS);
	BlockData dataMountain = BlockData::Get(STONE);

	while (getline(fin, line))
	{
		x = 0;
		std::stringstream s(line);

		while (getline(s, word, ','))
		{
			value = atoi(word.c_str());
			// Sample tree noise
			treeNoiseValue = (perlin.noise2D(x / scale * 2, y / scale * 2) + 1) / 2;

			auto block = GetCube(x, 0, y);
			*block = SAND;

			switch (value) {
			case 1:
				yMax = 0;
				break;
			case 0:
				yMax = 1;
				break;
			default:
				yMax = value;
				break;
			}

			for (int up = 1; up <= yMax; up++) {
				auto block = GetCube(x, up, y);
				*block = up < 3 ? GRASS : STONE;
			}

			if (yMax == 0) {
				// Add water
				auto block = GetCube(x, 1, y);
				*block = WATER;
			}
			else if(treeNoiseValue <= treeThreshold && yMax <= 2) {
				// Place tree
				PlaceBuilding(TREE, x, yMax+1, y);
			}

			x++;
		}

		y++;
	}

	Create(deviceRes);
}

void World::Draw(Camera* camera, DeviceResources* deviceRes) {
	auto gpuRes = DefaultResources::Get();
	gpuRes->cbModel.ApplyToVS(deviceRes, 0);

	for (int pass = SP_OPAQUE; pass < SP_COUNT; pass++) {
		switch (pass) {
		case SP_OPAQUE:
			gpuRes->opaque.Apply(deviceRes);
			gpuRes->defaultDepth.Apply(deviceRes);
			break;
		case SP_TRANSPARENT:
			gpuRes->alphaBlend.Apply(deviceRes);
			gpuRes->depthRead.Apply(deviceRes);
			break;
		}

		for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
			if (chunks[idx]->needRegen) chunks[idx]->Generate(deviceRes);

			if (chunks[idx]->bounds.Intersects(camera->bounds)) {
				gpuRes->cbModel.data.model = chunks[idx]->model.Transpose();
				gpuRes->cbModel.data.isInstance = false;
				gpuRes->cbModel.UpdateBuffer(deviceRes);
				chunks[idx]->Draw(deviceRes, (ShaderPass)pass);
			}
		}
	}

	// Clean

	gpuRes->cbModel.data.model = Matrix::Identity;
	gpuRes->cbModel.UpdateBuffer(deviceRes);
}

void World::DrawBuildings(Camera* camera, DeviceResources* deviceRes)
{
	auto gpuRes = DefaultResources::Get();
	gpuRes->cbModel.ApplyToVS(deviceRes, 0);

	// Render buildings
	gpuRes->opaque.Apply(deviceRes);
	gpuRes->defaultDepth.Apply(deviceRes);
	Building keys[] = { TREE,ROAD,HOUSE,SHOP,FACTORY,ENERGYPLANT,WATERPLANT };
	for (Building key : keys) {

		if (buildingsPositions[key].positions->size() == 0) continue;

		//Set the models index buffer (same as before)
		gpuRes->cbModel.data.model = Matrix::Identity.Transpose();
		gpuRes->cbModel.data.isInstance = true;
		gpuRes->cbModel.UpdateBuffer(deviceRes);
		buildingsPositions[key].model->Draw(deviceRes, true);
	}
}

void World::Reset()
{
	passiveIncome = 0;
	energyGain = 0;
	waterGain = 0;

	// Reset buildings placements
	for (int x = 0; x < WORLD_SIZE * CHUNK_SIZE * WORLD_SIZE * CHUNK_SIZE; x++) {
		buildings[x] = NOTHING;
	}

	// ! Reset buildings positions !
	
	for (const auto& [key, value] : buildingsPositions) {
		value.positions->clear();
	}
	
	// Reset chunks
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				chunks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_HEIGHT]->Reset();
			}
		}
	}
	
}

Chunk* World::GetChunk(int cx, int cy, int cz) {
	if (cx < 0 || cy < 0 || cz < 0) return nullptr;
	if (cx > WORLD_SIZE - 1 || cy > WORLD_SIZE - 1 || cz > WORLD_SIZE - 1) return nullptr;
	return chunks[cx + cy * WORLD_SIZE + cz * WORLD_SIZE * WORLD_HEIGHT];
}

BlockId* World::GetCube(int gx, int gy, int gz) {
	int cx = gx / CHUNK_SIZE;
	int cy = gy / CHUNK_SIZE;
	int cz = gz / CHUNK_SIZE;
	int lx = gx % CHUNK_SIZE;
	int ly = gy % CHUNK_SIZE;
	int lz = gz % CHUNK_SIZE;

	Chunk* chunk = GetChunk(cx, cy, cz);
	if (!chunk) return nullptr;
	return chunk->GetCubeLocal(lx, ly, lz);
}

void World::MakeChunkDirty(int gx, int gy, int gz) {
	Chunk* chunk = GetChunkFromCoordinates(gx, gy, gz);
	if (chunk) chunk->needRegen = true;
}

bool World::IsAdjacentToWater(int gx, int gy, int gz)
{
	return *GetCube(gx + 1, gy, gz) == WATER || *GetCube(gx - 1, gy, gz) == WATER ||
		*GetCube(gx, gy, gz + 1) == WATER || *GetCube(gx, gy, gz - 1) == WATER;
}

Chunk* World::GetChunkFromCoordinates(int gx, int gy, int gz) {
	int cx = gx / CHUNK_SIZE;
	int cy = gy / CHUNK_SIZE;
	int cz = gz / CHUNK_SIZE;
	return GetChunk(cx, cy, cz);
}

void World::UpdateBlock(int gx, int gy, int gz, BlockId block) {
	BlockId* cube = GetCube(gx, gy, gz);
	if (!cube) return;
	*cube = block;

	MakeChunkDirty(gx, gy, gz);
	MakeChunkDirty(gx + 1, gy, gz);
	MakeChunkDirty(gx - 1, gy, gz);
	MakeChunkDirty(gx, gy + 1, gz);
	MakeChunkDirty(gx, gy - 1, gz);
	MakeChunkDirty(gx, gy, gz + 1);
	MakeChunkDirty(gx, gy, gz - 1);
}

Building World::GetBuilding(int x, int y)
{
	if (x < 0 || y < 0 || y >= CHUNK_SIZE * WORLD_SIZE || x >= CHUNK_SIZE * WORLD_SIZE) return NOTHING;
	return buildings[x + y * CHUNK_SIZE * WORLD_SIZE];
}

void World::PlaceBuilding(Building type, int x, int y, int z)
{

	buildings[x + z * CHUNK_SIZE * WORLD_SIZE] = type;
	buildingsPositions[type].positions->push_back(Vector3(x,y,z));
	
	energyGain += buildingsPositions[type].energy;
	waterGain += buildingsPositions[type].water;

	// Checks to know if the building can generate income / help other building generate income
	Building typeAdjacent;
	if (type == ROAD) {
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if ((j == 0 || i == 0) && i != j) {
					typeAdjacent = GetBuilding(x + i, z + j);

					// If there is only 1 road next to this building, then it can generate income
					if (typeAdjacent != NOTHING && GetAmountOfAdjacentRoads(x + i, z + j) == 1) passiveIncome += buildingsPositions[typeAdjacent].income;
				}
			}
		}
	}
	else {
		if (GetAmountOfAdjacentRoads(x, z) > 0) passiveIncome += buildingsPositions[type].income;
	}

	RegenerateBufferFor(type);
}

void World::RemoveBuilding(int x, int y, int z)
{

	Building type = GetBuilding(x, z);

	int size = buildingsPositions[type].positions->size();
	Vector3 position;
	for (int i = 0; i < size; i++) {
		position = buildingsPositions[type].positions->at(i);
		if ((int)position.x == x && (int)position.y == y && (int)position.z == z) {
			buildingsPositions[type].positions->erase(buildingsPositions[type].positions->begin() + i);

			buildings[x + z * CHUNK_SIZE * WORLD_SIZE] = NOTHING;
			energyGain -= buildingsPositions[type].energy;
			waterGain -= buildingsPositions[type].water;

			// If the building is a road, check the nearby buildings. They may not be connected to a road anymore
			Building typeAdjacent;
			if (type == ROAD) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if ((j == 0 || i == 0) && i != j) {
							typeAdjacent = GetBuilding(x + i, z + j);
							// If there is 0 road next to this building, then it cannot generate income
							if (typeAdjacent != NOTHING && GetAmountOfAdjacentRoads(x + i, z + j) == 0) passiveIncome -= buildingsPositions[typeAdjacent].income;
						}
					}
				}
			}
			else {
				if (GetAmountOfAdjacentRoads(x, z) > 0) passiveIncome -= buildingsPositions[type].income;
			}

			RegenerateBufferFor(type);

			return;
		}
	}
}

int World::GetWaterDelta()
{
	return waterGain;
}

int World::GetEnergyDelta()
{
	return energyGain;
}

int World::GetPassiveIncome()
{
	int result = passiveIncome;
	if (energyGain < 0) result = result / 2.0f;
	if (waterGain < 0) result = result / 2.0f;
	return result + 1;
}

int World::GetAmountOfAdjacentRoads(int x, int y)
{
	int total = 0;
	int mapSize = CHUNK_SIZE * WORLD_SIZE;
	
	if (x > 0 && buildings[(x - 1) + y * mapSize] == ROAD) total++;
	if (y > 0 && buildings[x + (y-1) * mapSize] == ROAD) total++;
	if (x < mapSize-1 && buildings[(x + 1) + y * mapSize] == ROAD) total++;
	if (y < mapSize-1 && buildings[x + (y + 1) * mapSize] == ROAD) total++;

	return total;
}

void World::RegenerateBufferFor(Building building)
{
	buildingsPositions[building].model->ResetInstanceBuffer(deviceRes, buildingsPositions[building].positions);
}

void World::Create(DeviceResources* deviceRes)
{
	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++)
		chunks[idx]->Generate(deviceRes);

	buildingsPositions[TREE].model->Generate(deviceRes);
	buildingsPositions[HOUSE].model->Generate(deviceRes);
	buildingsPositions[SHOP].model->Generate(deviceRes);
	buildingsPositions[FACTORY].model->Generate(deviceRes);
	buildingsPositions[WATERPLANT].model->Generate(deviceRes);
	buildingsPositions[ENERGYPLANT].model->Generate(deviceRes);
	buildingsPositions[ROAD].model->Generate(deviceRes);

	DefaultResources::Get()->cbModel.Create(deviceRes);
}
