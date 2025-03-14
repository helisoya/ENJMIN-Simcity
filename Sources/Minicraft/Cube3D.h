#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/World.h"
#include "Minicraft/Block.h"

class Cube3D {
	BlockId blockId;

	VertexBuffer<VertexLayout_PositionNormalUV> vb;
	VertexBuffer<Vector3> instbuffer;
	IndexBuffer ib;

	bool needRegen = true;
public:
	Matrix model = Matrix::Identity;

	Cube3D(BlockId id) : blockId(id) {}

	BlockId GetBlockId() const { return blockId; }
	void SetBlockId(const BlockId& id) { blockId = id; needRegen = true; }

	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes, bool isInstanced = false);
	void ResetInstanceBuffer(DeviceResources* deviceRes, std::vector<Vector3>* positions);

private:
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id, bool front = true);
	void PushTriangle(Vector3 a, Vector3 b, Vector3 c, Vector3 normal, int id, bool front = true);
};