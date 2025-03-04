#include "pch.h"

#include "Engine/DefaultResources.h"
#include "World.h"
#include "PerlinNoise.hpp"
#include "iostream"
#include "fstream"
#include "sstream"

World::World() {
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

	BuildingData data = {};
	data.model = new Cube3D(LOG);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[TREE] = data;

	data = {};
	data.model = new Cube3D(WOOD);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[HOUSE] = data;

	data = {};
	data.model = new Cube3D(SLAB);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[SHOP] = data;

	data = {};
	data.model = new Cube3D(BRICK);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[FACTORY] = data;

	data = {};
	data.model = new Cube3D(DIAMOND_BLOCK);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[WATERPLANT] = data;

	data = {};
	data.model = new Cube3D(GOLD_BLOCK);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[ENERGYPLANT] = data;

	data = {};
	data.model = new Cube3D(OBSIDIAN);
	data.positions = new std::vector<Vector3>();
	buildingsPositions[ROAD] = data;
}

World::~World() {
	for (int idx = 0; idx < WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT; idx++) {
		delete chunks[idx];
		chunks[idx] = nullptr;
	}
}

void World::Generate(DeviceResources* deviceRes) {
	siv::BasicPerlinNoise<float> perlin(786768768876);
	float noiseValue;
	int yMax;

	float scale = WORLD_SIZE * CHUNK_SIZE / 2.5;


	for (int x = 0; x < CHUNK_SIZE * WORLD_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE * WORLD_SIZE; z++) {

			noiseValue = (perlin.noise2D(x / scale, z / scale) + 1) / 2;
			yMax = (int)(noiseValue * 5);

			if (yMax == 1) {
				auto block = GetCube(x, 1, z);
				*block = WATER;
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
		}
	}

	Create(deviceRes);
}

void World::GenerateFromFile(DeviceResources* deviceRes, std::wstring filePath)
{


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
				auto block = GetCube(x, 1, y);
				*block = WATER;
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
				gpuRes->cbModel.UpdateBuffer(deviceRes);
				chunks[idx]->Draw(deviceRes, (ShaderPass)pass);
			}
		}
	}

	// Render buildings
	gpuRes->opaque.Apply(deviceRes);
	gpuRes->defaultDepth.Apply(deviceRes);
	Building keys[] = { TREE,ROAD,HOUSE,SHOP,FACTORY,ENERGYPLANT,WATERPLANT};
	for (Building key : keys) {
		for (Vector3 position : *(buildingsPositions[key].positions)) {
			gpuRes->cbModel.data.model = Matrix::CreateTranslation(position).Transpose();
			gpuRes->cbModel.UpdateBuffer(deviceRes);
			buildingsPositions[key].model->Draw(deviceRes);
		}

	}


	// Clean

	gpuRes->cbModel.data.model = Matrix::Identity;
	gpuRes->cbModel.UpdateBuffer(deviceRes);
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
