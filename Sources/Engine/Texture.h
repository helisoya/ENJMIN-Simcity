#pragma once

using Microsoft::WRL::ComPtr;

/// <summary>
/// Represents a texture
/// </summary>
class Texture {
	std::wstring textureName;

	ComPtr<ID3D11ShaderResourceView> textureRV;
	ComPtr<ID3D11SamplerState> samplerState;
public:
	Texture(std::wstring name) : textureName(name) {};

	/// <summary>
	/// Creates the texture
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes);

	/// <summary>
	/// Applies the texture
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Apply(DeviceResources* deviceRes);
};