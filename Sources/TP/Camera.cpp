#include "pch.h"

#include "Camera.h"
#include "World.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : fov(fov) {
	// TP initialiser matrices
	camPos = Vector3(20, 15, 20);

	camRot = Quaternion::CreateFromYawPitchRoll(0,-45,0);
	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);

	view = Matrix::CreateLookAt(camPos, camPos + newForward, newUp);
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}

Vector3& Camera::GetPosition()
{
	return camPos;
}

DirectX::BoundingFrustum Camera::GetFrostum()
{
	return frustum;
}

void Camera::UpdateAspectRatio(float aspectRatio) {
	// TP updater matrice proj
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse, World& world) {
	float camSpeedRot = 0.10f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix viewInverse = view.Invert();

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	camPos += delta * camSpeed * dt;

	float scrollValue = -((float)mstate.scrollWheelValue * 0.25f);
	camPos += Vector3::Up * scrollValue * camSpeed * dt;
	camPos.y = std::clamp(camPos.y, 8.0f, 50.0f);
	mouse->ResetScrollWheelValue();

	if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;

		lastMouseX += dx;
		lastMouseY += dy;

		if (mstate.leftButton) {
			//camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -dx * dt);
		}
		else if(!mstate.rightButton){
			mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
	}
	else if (mstate.rightButton || mstate.leftButton) {
		mouse->SetMode(Mouse::MODE_RELATIVE);
	}

	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);

	view = Matrix::CreateLookAt(camPos, camPos + newForward, newUp);

	BoundingFrustum::CreateFromMatrix(frustum, projection, true);
	frustum.Transform(frustum, view.Invert());


	if (kb.D1) currentIdxBlock = 0;
	if (kb.D2) currentIdxBlock = 1;
	if (kb.D3) currentIdxBlock = 2;
	if (kb.D4) currentIdxBlock = 3;
	if (kb.D5) currentIdxBlock = 4;
	if (kb.D6) currentIdxBlock = 5;
	if (kb.D7) currentIdxBlock = 6;
	if (kb.D8) currentIdxBlock = 7;

	if (kb.P) {
		if (!isBuilding) {
			isBuilding = true;

			Matrix viewProjInvert = (view * projection).Invert().Transpose();

			Vector3 nearVec(lastMouseX, -lastMouseY, 0);
			Vector3 farVec(lastMouseX, -lastMouseY, 1);

			Vector3 rayOrigin = XMVector3TransformCoord(nearVec, viewProjInvert);
			Vector3 rayEnd = XMVector3TransformCoord(farVec, viewProjInvert);
			Vector3 rayDir = (rayEnd - rayOrigin);
			rayDir.Normalize();
			
			world.SearchForBlock(camPos, newForward, blocksToPlace[currentIdxBlock]);
		}
	}
	else {
		isBuilding = false;
	}
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!cbCamera) {
		cbCamera = new ConstantBuffer<MatrixData>();
		cbCamera->Create(deviceRes);
	}

	// TP envoyer data

	MatrixData data = {};
	data.mView = view.Transpose();
	data.mProjection = projection.Transpose();

	cbCamera->SetData(data, deviceRes);

	cbCamera->ApplyToVS(deviceRes, 1);
}