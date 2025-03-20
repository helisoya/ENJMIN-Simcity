#pragma once

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

/// <summary>
/// Represents a shader
/// </summary>
class Shader {
	std::wstring shaderName;
	std::vector<uint8_t> psBytecode;

	ComPtr<ID3D11VertexShader>	vertexShader;
	ComPtr<ID3D11PixelShader>	pixelShader;
public:
	std::vector<uint8_t> vsBytecode;
	Shader(std::wstring name) : shaderName(name) {};

	/// <summary>
	/// Creates the shader
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes);

	/// <summary>
	/// Applies the shader
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Apply(DeviceResources* deviceRes);
};

extern std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> g_inputLayouts;

/// <summary>
/// Generates an input layout
/// </summary>
/// <typeparam name="T">The input layout's elements</typeparam>
/// <param name="deviceRes">The game's device resources</param>
/// <param name="basicShader">The linked shader</param>
template <typename T>
void GenerateInputLayout(DeviceResources* deviceRes, Shader* basicShader) {
	auto it = g_inputLayouts.find(typeid(T).name());
	if (it != g_inputLayouts.end()) return;
	deviceRes->GetD3DDevice()->CreateInputLayout(
		typename T::InputElementDescs.data(),
		typename T::InputElementDescs.size(),
		basicShader->vsBytecode.data(),
		basicShader->vsBytecode.size(),
		g_inputLayouts[typeid(T).name()].ReleaseAndGetAddressOf());
}

/// <summary>
/// Applies an input layout
/// </summary>
/// <typeparam name="T">The input layout's elements</typeparam>
/// <param name="deviceRes">The game's device resources</param>
template <typename T>
void ApplyInputLayout(DeviceResources* deviceRes) {
	auto it = g_inputLayouts.find(typeid(T).name());
	assert(it != g_inputLayouts.end());
	deviceRes->GetD3DDeviceContext()->IASetInputLayout(it->second.Get());
}