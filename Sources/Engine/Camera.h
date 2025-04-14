#pragma once

#include "Engine/Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/// <summary>
/// Represents a Camera
/// </summary>
class Camera {
protected:
	float nearPlane = 0.01f;
	float farPlane = 500.0f;
	
	Vector3 camPos = Vector3(0, 0, 2);
	Quaternion camRot = Quaternion();
	Matrix projection;
	Matrix view;

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
	};
	ConstantBuffer<MatrixData>* cbCamera = nullptr;

	/// <summary>
	///  Updates the camera's view matrix
	/// </summary>
	void UpdateViewMatrix();
public:
	DirectX::BoundingFrustum bounds;

	Camera();
	virtual ~Camera();

	// Gets the camera's position
	Vector3 GetPosition() const { return camPos; };

	// Gets the camera's rotation
	Quaternion GetRotation() const { return camRot; };

	/// <summary>
	/// Sets the camera's position
	/// </summary>
	/// <param name="pos">Its new position</param>
	void SetPosition(const Vector3& pos) { camPos = pos; UpdateViewMatrix(); };

	/// <summary>
	/// Sets the camera's rotation
	/// </summary>
	/// <param name="rot">Its new rotation</param>
	void SetRotation(const Quaternion& rot) { camRot = rot; UpdateViewMatrix(); };

	// Forward vector of the camera
	Vector3 Forward() const { return Vector3::TransformNormal(Vector3::Forward, view.Invert()); }
	// Up vector of the camera
	Vector3 Up() const { return Vector3::TransformNormal(Vector3::Up, view.Invert()); }
	// Right vector of the camera
	Vector3 Right() const { return Vector3::TransformNormal(Vector3::Right, view.Invert()); }

	// Gets the view matrix of the camera
	Matrix GetViewMatrix() const { return view; }
	// Gets the inverse view matrix of the camera
	Matrix GetInverseViewMatrix() const { return view.Invert(); }

	// Gets the projection matrix of the camera
	Matrix GetProjectionMatrix() const { return projection; }
	// Gets the inverse projection matrix of the camera
	Matrix GetInverseProjectionMatrix() const { return projection.Invert(); }

	/// <summary>
	/// Applies the camera
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void ApplyCamera(DeviceResources* deviceRes);
};

/// <summary>
/// Represents a perspective camera
/// </summary>
class PerspectiveCamera : public Camera {
	float fov;
public:
	PerspectiveCamera(float fov, float aspectRatio);

	/// <summary>
	/// Updates the camera's aspect ratio
	/// </summary>
	/// <param name="aspectRatio">Its new aspect ratio</param>
	void UpdateAspectRatio(float aspectRatio);
};

/// <summary>
/// Represents an orthographic camera
/// </summary>
class OrthographicCamera : public Camera {
	float width, height;
public:
	OrthographicCamera(float width, float height);

	/// <summary>
	/// Updates the camera's size
	/// </summary>
	/// <param name="width">The new width</param>
	/// <param name="height">The new height</param>
	void UpdateSize(float width, float height);
};