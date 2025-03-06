#pragma once

#include "Engine/DepthState.h"
#include "Engine/Camera.h"
#include "Engine/StepTimer.h"
#include "Minicraft/World.h"
#include "Minicraft/Cube3D.h"

using namespace DirectX::SimpleMath;

class Player {
	World* world = nullptr;

	Vector3 position = Vector3();
	float velocityY = 0;

	float walkSpeed = 10.0f;

	PerspectiveCamera camera = PerspectiveCamera(75, 1);

	Cube3D highlightCube = Cube3D(WOOL);
	Building possibleBuildings[7] = {NOTHING,ROAD,HOUSE,SHOP,FACTORY,ENERGYPLANT,WATERPLANT};
	char* buildingsNames[7] = { "Destroy","Road","House","Shop","Factory","Energy Plant","Water Plant" };
	int prices[7] = {25,2,4,6,10,15,15};
	int currentBuildingIdx = 0;
	int money = 100;

	float passiveIncomeCooldown = 10;

	DirectX::Mouse::ButtonStateTracker      mouseTracker;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
public:
	Player(World* w, Vector3 pos) : world(w), position(pos){}

	void GenerateGPUResources(DeviceResources* deviceRes);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms);
	void Draw(DeviceResources* deviceRes);
	void Im(DX::StepTimer const& timer);

	PerspectiveCamera* GetCamera() { return &camera; }
};
