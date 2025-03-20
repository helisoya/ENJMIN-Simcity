#pragma once

using Microsoft::WRL::ComPtr;

/// <summary>
/// Represents a depth state
/// </summary>
class DepthState {
	CD3D11_DEPTH_STENCIL_DESC desc;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
public:
	DepthState() : desc(D3D11_DEFAULT) {};
	DepthState(bool depthRead, bool depthWrite, D3D11_COMPARISON_FUNC func = D3D11_COMPARISON_LESS);
	
	/// <summary>
	/// Creates the depth state
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes);

	/// <summary>
	/// Applies the depth state
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Apply(DeviceResources* deviceRes);
};