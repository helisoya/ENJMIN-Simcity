#pragma once

using Microsoft::WRL::ComPtr;

/// <summary>
/// Represents a way of blending
/// </summary>
class BlendState {
	CD3D11_BLEND_DESC desc;
	ComPtr<ID3D11BlendState> blendState;
public:
	BlendState() : desc(D3D11_DEFAULT) {};
	BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op);
	BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op, D3D11_BLEND srcAlpha, D3D11_BLEND dstAlpha, D3D11_BLEND_OP opAlpha);
	
	/// <summary>
	/// Creates the blendState
	/// </summary>
	/// <param name="deviceRes">The Game's Device Resources</param>
	void Create(DeviceResources* deviceRes);

	/// <summary>
	/// Apply the blendState
	/// </summary>
	/// <param name="deviceRes">The Game's Device Resources</param>
	void Apply(DeviceResources* deviceRes);
};