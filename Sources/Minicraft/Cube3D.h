#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/World.h"
#include "Minicraft/Block.h"

/// <summary>
/// Represents a 3D Model
/// In the case of this game, it's the building models
/// </summary>
class Cube3D {
	Building buildingType;

	VertexBuffer<VertexLayout_PositionNormalUV> vb;
	VertexBuffer<Vector3> instbuffer;
	IndexBuffer ib;

	bool needRegen = true;
public:
	Matrix model = Matrix::Identity;

	Cube3D(Building type) : buildingType(type) {}

	// Gets the linked building of this model
	Building GetBuilding() const { return buildingType; }

	/// <summary>
	/// Sets the building type of this model
	/// </summary>
	/// <param name="type">Its new type</param>
	void SetBuilding(const Building& type) { this->buildingType = type; needRegen = true; }

	/// <summary>
	/// Generates the model
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Generate(DeviceResources* deviceRes);

	/// <summary>
	/// Draws the model
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="isInstanced">Use instancing</param>
	void Draw(DeviceResources* deviceRes, bool isInstanced = false);

	/// <summary>
	/// Resets the instance buffer's positions
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="positions">The instanced positions</param>
	void ResetInstanceBuffer(DeviceResources* deviceRes, std::vector<Vector3>* positions);

private:

	/// <summary>
	/// Pushs a face to the model's buffers
	/// </summary>
	/// <param name="pos">The face's position</param>
	/// <param name="up">The face's up vector</param>
	/// <param name="right">The face's right vector</param>
	/// <param name="normal">The face's normal</param>
	/// <param name="id">The block's ID</param>
	/// <param name="front">True if it is a front face</param>
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id, bool front = true);

	/// <summary>
	/// Pushs a triangle to the model's buffers
	/// </summary>
	/// <param name="a">Point A's position</param>
	/// <param name="b">Point B's position</param>
	/// <param name="c">Point C's position</param>
	/// <param name="normal">The face's normal</param>
	/// <param name="id">The block's ID</param>
	/// <param name="front">True if it is a front face</param>
	void PushTriangle(Vector3 a, Vector3 b, Vector3 c, Vector3 normal, int id, bool front = true);
};