#pragma once

#include "TP/Buffer.h"
#include "Block.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class World;
class Camera {
	float fov;
	float nearPlane = 0.01f;
	float farPlane = 500.0f;

	Vector3 camPos = Vector3(0, 0, 0);
	Quaternion camRot = Quaternion::Identity;
	Matrix projection;
	Matrix view;

	int lastMouseX = 0;
	int lastMouseY = 0;

	bool isBuilding = false;
	int currentIdxBlock = 0;
	BlockId blocksToPlace[8] = { EMPTY, DIRT, STONE,COBBLESTONE,WOOD,GLASS,CRAFTING_TABLE,DIAMOND_BLOCK};

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
	};
	ConstantBuffer<MatrixData>* cbCamera = nullptr;

	DirectX::BoundingFrustum frustum;
public:
	Camera(float fov, float aspectRatio);
	~Camera();


	Vector3& GetPosition();
	DirectX::BoundingFrustum GetFrostum();

	void UpdateAspectRatio(float aspectRatio);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse* ms, World& world);

	void ApplyCamera(DeviceResources* deviceRes);
};