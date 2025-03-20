#pragma once

using Microsoft::WRL::ComPtr;

/// <summary>
/// Represents a VertexBuffer
/// </summary>
/// <typeparam name="TVertex">The vertex's type</typeparam>
template<typename TVertex>
class VertexBuffer {
	ComPtr<ID3D11Buffer> buffer;
public:
	std::vector<TVertex> data;
	VertexBuffer() {};

	/// <summary>
	/// Pushs a vertex into the buffer
	/// </summary>
	/// <param name="v">The vertex</param>
	/// <returns>The vertex's index</returns>
	uint32_t PushVertex(TVertex v) {
		data.push_back(v);
		return data.size() - 1;
	}

	/// <summary>
	/// Clears the buffer
	/// </summary>
	void Clear() {
		data.clear();
	}

	/// <summary>
	/// Gets the buffer's pointer
	/// </summary>
	/// <returns>The pointer</returns>
	ComPtr<ID3D11Buffer> get(){
		return buffer;
	}

	/// <summary>
	/// Gets the buffer's size
	/// </summary>
	/// <returns>Its size</returns>
	size_t Size() {
		return data.size();
	}

	/// <summary>
	/// Creates the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes) {
		buffer.Reset();
		if (data.size() == 0) return;
		CD3D11_BUFFER_DESC desc(
			sizeof(TVertex) * data.size(),
			D3D11_BIND_VERTEX_BUFFER
		);
		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = data.data();

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.GetAddressOf()
		);
	}

	/// <summary>
	/// Updates the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void UpdateBuffer(DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}

	/// <summary>
	/// Applies the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="slot">The buffer's slot</param>
	void Apply(DeviceResources* deviceRes, int slot = 0) {
		ID3D11Buffer* vbs[] = { buffer.Get() };
		const UINT strides[] = { sizeof(TVertex) };
		const UINT offsets[] = { 0 };
		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(slot, 1, vbs, strides, offsets);
	}
};

/// <summary>
/// Represents an index buffer
/// </summary>
class IndexBuffer {
	ComPtr<ID3D11Buffer> buffer;
	std::vector<uint32_t> indices;
public:
	IndexBuffer() {};

	/// <summary>
	/// Pushes a new triangle in the buffer
	/// </summary>
	/// <param name="a">Point A's index</param>
	/// <param name="b">Point B's index</param>
	/// <param name="c">Point C's index</param>
	void PushTriangle(uint32_t a, uint32_t b, uint32_t c) {
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	/// <summary>
	/// Clears the buffer
	/// </summary>
	void Clear() {
		indices.clear();
	}
	
	/// <summary>
	/// Gets the buffer's size
	/// </summary>
	/// <returns>Its size</returns>
	size_t Size() {
		return indices.size();
	}

	/// <summary>
	/// Creates the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes) {
		buffer.Reset();
		if (indices.size() == 0) return;
		CD3D11_BUFFER_DESC desc(
			sizeof(uint32_t) * indices.size(),
			D3D11_BIND_INDEX_BUFFER
		);
		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = indices.data();

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.GetAddressOf()
		);
	}

	/// <summary>
	/// Applies the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Apply(DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
};

/// <summary>
/// Represents a constant buffer
/// </summary>
/// <typeparam name="TData">The data's type</typeparam>
template<typename TData>
class ConstantBuffer {
	ComPtr<ID3D11Buffer> buffer;
public:
	TData data;

	ConstantBuffer() {};

	/// <summary>
	/// Creates the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc(sizeof(TData), D3D11_BIND_CONSTANT_BUFFER);
		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc, nullptr,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	/// <summary>
	/// Updates the buffer
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	void UpdateBuffer(DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}
	
	/// <summary>
	/// Applies the buffer to the vertex shader
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="slot">The buffer's slot</param>
	void ApplyToVS(DeviceResources* deviceRes, int slot = 0) {
		ID3D11Buffer* cbs[] = { buffer.Get() };
		deviceRes->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
	}

	/// <summary>
	/// Applies the buffer to the pixel shader
	/// </summary>
	/// <param name="deviceRes">The game's device resources</param>
	/// <param name="slot">The buffer's slot</param>
	void ApplyToPS(DeviceResources* deviceRes, int slot = 0) {
		ID3D11Buffer* cbs[] = { buffer.Get() };
		deviceRes->GetD3DDeviceContext()->PSSetConstantBuffers(slot, 1, cbs);
	}
};