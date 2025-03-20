#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/World.h"
#include "Minicraft/Block.h"

#define CHUNK_SIZE 16
class World;

/// <summary>
/// Represents a chunck of the world
/// </summary>
class Chunk {
	BlockId data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	World* world;

	VertexBuffer<VertexLayout_PositionNormalUV> vb[SP_COUNT];
	IndexBuffer ib[SP_COUNT];

	Chunk* adjXPos = nullptr;
	Chunk* adjXNeg = nullptr;
	Chunk* adjYPos = nullptr;
	Chunk* adjYNeg = nullptr;
	Chunk* adjZPos = nullptr;
	Chunk* adjZNeg = nullptr;
public:
	Matrix model;
	DirectX::BoundingBox bounds;
	bool needRegen = false;

	Chunk(World* world, Vector3 pos);

	/// <summary>
	/// Generates the chunk
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Generate(DeviceResources* deviceRes);

	/// <summary>
	/// Draws the chunk
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="pass">The linked shader pass</param>
	void Draw(DeviceResources* deviceRes, ShaderPass pass);

	/// <summary>
	/// Gets a local cube in the chunk 
	/// </summary>
	/// <param name="lx">The cube's X position</param>
	/// <param name="ly">The cube's Y position</param>
	/// <param name="lz">The cube's Z position</param>
	/// <returns>The cube</returns>
	BlockId* GetCubeLocal(int lx, int ly, int lz);

	// Reset the chunk
	void Reset();
private:

	/// <summary>
	/// Pushs a cube inside the chunk
	/// </summary>
	/// <param name="lx">The cube's X position</param>
	/// <param name="ly">The cube's Y position</param>
	/// <param name="lz">The cube's Z position</param>
	void PushCube(int x, int y, int z);

	/// <summary>
	/// Pushs a face to the chunk's buffers
	/// </summary>
	/// <param name="pos">The face's position</param>
	/// <param name="up">The face's up vector</param>
	/// <param name="right">The face's right vector</param>
	/// <param name="normal">The face's normal</param>
	/// <param name="id">The block's ID</param>
	/// <param name="pass">The linked shader pass</param>
	/// <param name="scaleY">The Y scale</param>
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id, ShaderPass pass, float scaleY = 1.0f);

	/// <summary>
	/// Checks if a face should be rendered
	/// </summary>
	/// <param name="lx">The cube's X position</param>
	/// <param name="ly">The cube's Y position</param>
	/// <param name="lz">The cube's Y position</param>
	/// <param name="dx">The target's X position</param>
	/// <param name="dy">The target's Y position</param>
	/// <param name="dz">The target's Z position</param>
	/// <returns>True if the face should be rendered</returns>
	bool ShouldRenderFace(int lx, int ly, int lz, int dx, int dy, int dz);

	friend class World;
};