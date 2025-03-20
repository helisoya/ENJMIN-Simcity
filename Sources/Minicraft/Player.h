#pragma once

#include "Engine/DepthState.h"
#include "Engine/Camera.h"
#include "Engine/StepTimer.h"
#include "Minicraft/World.h"
#include "Minicraft/Cube3D.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// Represents the player
/// </summary>
class Player {
	World* world = nullptr;

	Vector3 position = Vector3();

	float walkSpeed = 10.0f;

	PerspectiveCamera camera = PerspectiveCamera(75, 1);

	Cube3D highlightCube = Cube3D(NOTHING);
	Building possibleBuildings[7] = {NOTHING,ROAD,HOUSE,SHOP,FACTORY,ENERGYPLANT,WATERPLANT};
	char* buildingsNames[7] = { "Destroy","Road","House","Shop","Factory","Energy Plant","Water Plant" };
	int prices[7] = {25,2,4,6,10,15,15};
	int currentBuildingIdx = 0;
	int money = 100;

	float currentYaw = 0;

	float passiveIncomeCooldown = 10;

	DirectX::Mouse::ButtonStateTracker      mouseTracker;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
public:
	Player(World* w, Vector3 pos) : world(w), position(pos){}

	/// <summary>
	/// Generates the player's resources
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void GenerateGPUResources(DeviceResources* deviceRes);

	/// <summary>
	/// Updates the player
	/// </summary>
	/// <param name="dt">The delta time</param>
	/// <param name="kb">The keyboard's state</param>
	/// <param name="ms">The mouse's state</param>
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms);

	/// <summary>
	/// Draws the player
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Draw(DeviceResources* deviceRes);

	// Resets the player
	void Reset();

	// ImGui pass for the player
	void Im(DX::StepTimer const& timer);

	// Gets the player's camera
	PerspectiveCamera* GetCamera() { return &camera; }
};
