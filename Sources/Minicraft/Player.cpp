#include "pch.h"

#include "Engine/DefaultResources.h"
#include "Player.h"
#include "Utils.h"

using ButtonState = Mouse::ButtonStateTracker::ButtonState;

void Player::GenerateGPUResources(DeviceResources* deviceRes) {
	currentCube.Generate(deviceRes);
	highlightCube.Generate(deviceRes);

	camera.SetRotation(Quaternion::CreateFromYawPitchRoll(0,-45,0));
}

void Player::Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms) {
	keyboardTracker.Update(kb);
	mouseTracker.Update(ms);

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	position += delta * walkSpeed * dt;

	float scrollValue = -((float)ms.scrollWheelValue * 0.25f);
	position += Vector3::Up * scrollValue * walkSpeed * dt;
	position.y = std::clamp(position.y, 8.0f, 50.0f);

	camera.SetPosition(position + Vector3(0, 1.25f, 0));
	highlightCube.model = Matrix::Identity;

	auto cubes = Raycast(camera.GetPosition(), camera.Forward(), 120);
	for (int i = 0; i < cubes.size(); i++) {
		BlockId* block = world->GetCube(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (block == nullptr || cubes[i][1] >= 16 || cubes[i][1] < 0) continue;
		BlockData blockData = BlockData::Get(*block);
		if (blockData.flags & BF_NO_RAYCAST) continue;

		highlightCube.model = Matrix::CreateTranslation(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (mouseTracker.leftButton == ButtonState::PRESSED) {
			world->UpdateBlock(cubes[i][0], cubes[i][1], cubes[i][2], EMPTY);
		} else if(mouseTracker.rightButton == ButtonState::PRESSED && i > 0) {
			if (blockData.flags & BF_HALF_BLOCK && *block == currentCube.GetBlockId()) {
				world->UpdateBlock(cubes[i][0], cubes[i][1], cubes[i][2], (BlockId)((int)currentCube.GetBlockId() + 1));
			} else {
				world->UpdateBlock(cubes[i - 1][0], cubes[i - 1][1], cubes[i - 1][2], currentCube.GetBlockId());
			}
		}
		break;
	}

}

void Player::Draw(DeviceResources* deviceRes) {
	auto gpuRes = DefaultResources::Get();

	gpuRes->noDepth.Apply(deviceRes);
	gpuRes->cbModel.ApplyToVS(deviceRes, 0);


	gpuRes->depthEqual.Apply(deviceRes);
	gpuRes->cbModel.data.model = highlightCube.model.Transpose();
	gpuRes->cbModel.UpdateBuffer(deviceRes);
	highlightCube.Draw(deviceRes);

	gpuRes->cbModel.data.model = Matrix::Identity;
	gpuRes->cbModel.UpdateBuffer(deviceRes);
	gpuRes->defaultDepth.Apply(deviceRes);
}
