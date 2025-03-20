#pragma once

#include "Engine/Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/// <summary>
/// Represents a light source
/// </summary>
class Light {

public:

private:

	Vector3 position;
	Vector3 lookAt;
	Vector3 direction;
	
	Matrix view;
	Matrix projection;

	Color ambiant;
	Color diffuse;

	struct LightData {
		Vector4 LightPosition;
		Vector3 Direction;
		Color Ambiant;
		Color Diffuse;

		Matrix LightView;
		Matrix LightProjection;
		float pad;
	};

	ConstantBuffer<LightData> constantBufferLight;

public:

	Light();

	/// <summary>
	/// Generates the light source
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Generate(DeviceResources* deviceRes);

	/// <summary>
	/// Applies the light source
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Apply(DeviceResources* deviceRes);

	/// <summary>
	/// Generates the light source's view matrix
	/// </summary>
	void GenerateViewMatrix();

	/// <summary>
	/// Generate the light source's projection matrix
	/// </summary>
	/// <param name="screenDepth">The screen depth</param>
	/// <param name="screenNear">The screen's near plane</param>
	void GenerateProjectionMatrix(float screenDepth, float screenNear);

private:


};