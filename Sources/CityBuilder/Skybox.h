#pragma once

#include "Engine/VertexLayout.h"
#include "Engine/Buffers.h"

/// <summary>
/// Represents the game's skybox
/// </summary>
class Skybox {
public:

private:
	VertexBuffer<VertexLayout_PositionNormalUV> vertexBuffer;
	IndexBuffer indexBuffer;

public:

	Skybox();

	/// <summary>
	/// Generates the skybox
	/// </summary>
	/// <param name="deviceResources">The game's device resources</param>
	void Generate(DeviceResources* deviceResources);

	/// <summary>
	/// Pushs a face on the skybox
	/// </summary>
	/// <param name="pos">The face's position</param>
	/// <param name="up">The face's up vector</param>
	/// <param name="right">The face's right vector</param>
	/// <param name="uvIdx">The face's UV Index</param>
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx);

	/// <summary>
	/// Draws the skybox
	/// </summary>
	/// <param name="deviceResources">The game's device resources</param>
	void Draw(DeviceResources* deviceResources);

private:

	Vector4 ToVec4(const Vector3& v);
};