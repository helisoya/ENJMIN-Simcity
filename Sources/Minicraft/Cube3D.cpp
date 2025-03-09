#include "pch.h"

#include "Cube3D.h"
#include "Utils.h"

void Cube3D::PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id, bool front) {
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto a = vb.PushVertex({ ToVec4(pos), ToVec4Normal(normal), uv + Vector2::UnitY * BLOCK_TEXSIZE });
	auto b = vb.PushVertex({ ToVec4(pos + up), ToVec4Normal(normal), uv });
	auto c = vb.PushVertex({ ToVec4(pos + right), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE + Vector2::UnitY * BLOCK_TEXSIZE });
	auto d = vb.PushVertex({ ToVec4(pos + up + right), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE });
	if (front) {
		ib.PushTriangle(a, b, c);
		ib.PushTriangle(c, b, d);
	}
	else {
		ib.PushTriangle(a, c, b);
		ib.PushTriangle(c, d, b);
	}

}

void Cube3D::PushTriangle(Vector3 a, Vector3 b, Vector3 c, Vector3 normal, int id, bool front)
{
	Vector2 uv(
		(id % 16) * BLOCK_TEXSIZE,
		(id / 16) * BLOCK_TEXSIZE
	);

	auto aIdx = vb.PushVertex({ ToVec4(a), ToVec4Normal(normal), uv});
	auto bIdx = vb.PushVertex({ ToVec4(b), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE * 0.5f + Vector2::UnitY * BLOCK_TEXSIZE });
	auto cIdx = vb.PushVertex({ ToVec4(c), ToVec4Normal(normal), uv + Vector2::UnitX * BLOCK_TEXSIZE });

	if (front) {
		ib.PushTriangle(aIdx, bIdx, cIdx);
	}
	else {
		ib.PushTriangle(aIdx, cIdx, bIdx);
	}
}

void Cube3D::Generate(DeviceResources* deviceRes) {
	vb.Clear();
	ib.Clear();

	auto& data = BlockData::Get(blockId);
	float uvxSide = (data.texIdSide % 16) / BLOCK_TEXSIZE;
	float uvySide = (data.texIdSide / 16) / BLOCK_TEXSIZE;

	if (blockId == BLACKWOOL) {
		// Hard Code Roads for now
		PushFace({ -0.5f, -0.48f, -0.5f }, Vector3::Backward, Vector3::Right, Vector3::Down, data.texIdBottom,false);
	}
	else if (blockId == WOOD) {
		PushFace({ -0.3f, -0.5f,  0.3f }, Vector3::Up * 0.6f, Vector3::Right * 0.6f, Vector3::Backward, data.texIdSide);
		PushFace({ 0.3f, -0.5f,  0.3f }, Vector3::Up * 0.6f, Vector3::Forward * 0.6f, Vector3::Right, data.texIdSide);
		PushFace({ 0.3f, -0.5f, -0.3f }, Vector3::Up * 0.6f, Vector3::Left * 0.6f, Vector3::Forward, data.texIdSide);
		PushFace({ -0.3f, -0.5f, -0.3f }, Vector3::Up * 0.6f, Vector3::Backward * 0.6f, Vector3::Left, data.texIdSide);
		PushFace({ -0.3f, -0.5f, -0.3f }, Vector3::Backward * 0.6f, Vector3::Right * 0.6f, Vector3::Down, data.texIdBottom);

		PushTriangle({ 0.3f, 0.1f, -0.3f }, { 0.0f, 0.5f, -0.3f }, { -0.3f, 0.1f, -0.3f }, Vector3::Forward, data.texIdSide);
		PushTriangle({ 0.3f, 0.1f, 0.3f }, { -0.3f, 0.1f, 0.3f }, { 0.0f, 0.5f, 0.3f } , Vector3::Backward, data.texIdSide);
		PushFace({ -0.3f,  0.1f,  0.3f }, Vector3::Forward * 0.6f, Vector3::Right * 0.3f + Vector3::Up * 0.4f, Vector3::Up + Vector3::Left, data.texIdTop);
		PushFace({ 0.3f,  0.1f,  0.3f }, Vector3::Left * 0.3f + Vector3::Up * 0.4f, Vector3::Forward * 0.6f, Vector3::Up + Vector3::Right, data.texIdTop);

	}
	else if (blockId == LOG) {
		// Hard Code Tree for now

		PushFace({ -0.1f, -0.5f,  0.1f }, Vector3::Up * 0.2f, Vector3::Right * 0.2f, Vector3::Backward, data.texIdSide);
		PushFace({ 0.1f, -0.5f,  0.1f }, Vector3::Up * 0.2f, Vector3::Forward * 0.2f, Vector3::Right, data.texIdSide);
		PushFace({ 0.1f, -0.5f, -0.1f }, Vector3::Up * 0.2f, Vector3::Left * 0.2f, Vector3::Forward, data.texIdSide);
		PushFace({ -0.1f, -0.5f, -0.1f }, Vector3::Up * 0.2f, Vector3::Backward * 0.2f, Vector3::Left, data.texIdSide);
		PushFace({ -0.1f, -0.5f, -0.1f }, Vector3::Backward * 0.2f, Vector3::Right * 0.2f, Vector3::Down, data.texIdBottom);

		auto& data = BlockData::Get(GREENWOOL);

		PushFace({ -0.25f, -0.3f,  0.25f }, Vector3::Up * 0.5f, Vector3::Right * 0.5f, Vector3::Backward, data.texIdSide);
		PushFace({ 0.25f, -0.3f,  0.25f }, Vector3::Up * 0.5f, Vector3::Forward * 0.5f, Vector3::Right, data.texIdSide);
		PushFace({ 0.25f, -0.3f, -0.25f }, Vector3::Up * 0.5f, Vector3::Left * 0.5f, Vector3::Forward, data.texIdSide);
		PushFace({ -0.25f, -0.3f, -0.25f }, Vector3::Up * 0.5f, Vector3::Backward * 0.5f, Vector3::Left, data.texIdSide);
		PushFace({ -0.25f,  0.2f,  0.25f }, Vector3::Forward * 0.5f, Vector3::Right * 0.5f, Vector3::Up, data.texIdTop);
	}
	else {
		PushFace({ -0.5f, -0.5f,  0.5f }, Vector3::Up, Vector3::Right, Vector3::Backward, data.texIdSide);
		PushFace({ 0.5f, -0.5f,  0.5f }, Vector3::Up, Vector3::Forward, Vector3::Right, data.texIdSide);
		PushFace({ 0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Left, Vector3::Forward, data.texIdSide);
		PushFace({ -0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Backward, Vector3::Left, data.texIdSide);
		PushFace({ -0.5f,  0.5f,  0.5f }, Vector3::Forward, Vector3::Right, Vector3::Up, data.texIdTop);
		PushFace({ -0.5f, -0.5f, -0.5f }, Vector3::Backward, Vector3::Right, Vector3::Down, data.texIdBottom);
	}



	vb.Create(deviceRes);
	ib.Create(deviceRes);

	needRegen = false;
}

void Cube3D::Draw(DeviceResources* deviceRes, bool isInstanced) {
	if (needRegen)
		Generate(deviceRes);

	if (!isInstanced) {
		if (vb.Size() == 0) return;
		vb.Apply(deviceRes, 0);
		ib.Apply(deviceRes);
		deviceRes->GetD3DDeviceContext()->DrawIndexed(ib.Size(), 0, 0);
	}
	else {
		UINT strides[2] = { sizeof(VertexLayout_PositionNormalUV), sizeof(Vector3) };
		UINT offsets[2] = { 0, 0 };

		ID3D11Buffer* vertInstBuffers[2] = { vb.get().Get(), instbuffer.get().Get() };

		ib.Apply(deviceRes);
		//Set the models vertex buffer
		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(0, 2, vertInstBuffers, strides, offsets);
		deviceRes->GetD3DDeviceContext()->DrawIndexedInstanced(ib.Size(), instbuffer.Size(), 0, 0, 0);
	}


}

void Cube3D::ResetInstanceBuffer(DeviceResources* deviceRes, std::vector<Vector3>* positions)
{
	instbuffer.data = *positions;
	instbuffer.Create(deviceRes);
}
