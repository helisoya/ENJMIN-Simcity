#include "pch.h"

#include "Engine/DefaultResources.h"
#include "Player.h"
#include "Utils.h"
#include <Engine/StepTimer.h>
#include "string"
#include "iostream"

using ButtonState = Mouse::ButtonStateTracker::ButtonState;

void Player::GenerateGPUResources(DeviceResources* deviceRes) {
	highlightCube.Generate(deviceRes);

	camera.SetRotation(Quaternion::CreateFromYawPitchRoll(0,-45,0));
}

void Player::Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse::State ms) {
	keyboardTracker.Update(kb);
	mouseTracker.Update(ms);

	passiveIncomeCooldown -= dt;
	if (passiveIncomeCooldown <= 0) {
		passiveIncomeCooldown = 10.0f;
		money += world->GetPassiveIncome();
	}

	float speed = walkSpeed;
	if (kb.LeftShift) speed *= 2;

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	position += delta * speed * dt;

	float scrollValue = -((float)ms.scrollWheelValue * 0.25f);
	position += Vector3::Up * scrollValue * walkSpeed * dt;
	position.y = std::clamp(position.y, 8.0f, 50.0f);

	camera.SetPosition(position + Vector3(0, 1.25f, 0));
	highlightCube.model = Matrix::Identity;

	if (kb.D1) currentBuildingIdx = 0;
	else if (kb.D2) currentBuildingIdx = 1;
	else if (kb.D3) currentBuildingIdx = 2;
	else if (kb.D4) currentBuildingIdx = 3;
	else if (kb.D5) currentBuildingIdx = 4;
	else if (kb.D6) currentBuildingIdx = 5;
	else if (kb.D7) currentBuildingIdx = 6;

	auto cubes = Raycast(camera.GetPosition(), camera.Forward(), 100);
	for (int i = 0; i < cubes.size(); i++) {
		BlockId* block = world->GetCube(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (block == nullptr || cubes[i][1] >= 16 || cubes[i][1] < 0) continue;
		BlockData blockData = BlockData::Get(*block);
		if (blockData.flags & BF_NO_RAYCAST) continue;

		highlightCube.model = Matrix::CreateTranslation(cubes[i][0], cubes[i][1], cubes[i][2]);

		if ((cubes[i][1] != 1 && cubes[i][1] != 2)) continue;

		if (mouseTracker.leftButton == ButtonState::PRESSED && money >= prices[currentBuildingIdx]) {
			//world->UpdateBlock(cubes[i][0], cubes[i][1]+1, cubes[i][2], LOG);

			if (possibleBuildings[currentBuildingIdx] == WATERPLANT && !world->IsAdjacentToWater(cubes[i][0], cubes[i][1], cubes[i][2])) continue;
			if (possibleBuildings[currentBuildingIdx] == NOTHING && world->GetBuilding(cubes[i][0], cubes[i][2]) == NOTHING) continue;
			Building other = world->GetBuilding(cubes[i][0], cubes[i][2]);
			if (possibleBuildings[currentBuildingIdx] != NOTHING && world->GetBuilding(cubes[i][0], cubes[i][2]) != NOTHING) continue;

			if (possibleBuildings[currentBuildingIdx] != NOTHING) {
				money -= prices[currentBuildingIdx];
				world->PlaceBuilding(possibleBuildings[currentBuildingIdx], cubes[i][0], cubes[i][1] + 1, cubes[i][2]);
			}
			else {
				// Remove a building
				money -= prices[currentBuildingIdx];
				world->RemoveBuilding(cubes[i][0], cubes[i][1] + 1, cubes[i][2]);
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

void Player::Reset()
{
	money = 100;
	passiveIncomeCooldown = 10;
}

void Player::Im(DX::StepTimer const& timer)
{
	ImVec4 color;

	if (ImGui::CollapsingHeader("Player")) {
		ImGui::Text("Money : ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(money).c_str());
		ImGui::SameLine();
		ImGui::Text("(");
		ImGui::SameLine();
		ImGui::Text(std::to_string(world->GetPassiveIncome()).c_str());
		ImGui::SameLine();
		ImGui::Text(")");

		int energyValue = world->GetEnergyDelta();
		if(energyValue < 0) color = ImVec4(1, 0, 0, 1);
		else color = ImVec4(0, 1, 0, 1);

		ImGui::TextColored(color, "Energy : ");
		ImGui::SameLine();
		ImGui::TextColored(color, std::to_string(energyValue).c_str());


		int waterValue = world->GetWaterDelta();
		if (waterValue < 0) color = ImVec4(1, 0, 0, 1);
		else color = ImVec4(0, 1, 0, 1);

		ImGui::TextColored(color, "Water : ");
		ImGui::SameLine();
		ImGui::TextColored(color, std::to_string(waterValue).c_str());

		ImGui::Spacing();
		ImGui::Spacing();


		for (int i = 0; i < 7; i++) {
			if (i == currentBuildingIdx) color = ImVec4(1, 0, 0, 1);
			else if(prices[i] <= money) color = ImVec4(1, 1, 1, 1);
			else color = ImVec4(0.5f, 0.5f, 0.5f, 1);

			ImGui::TextColored(color, buildingsNames[i]);
			ImGui::SameLine();
			ImGui::TextColored(color, " - ");
			ImGui::SameLine();
			ImGui::TextColored(color, std::to_string(prices[i]).c_str());
		}
	}
}
