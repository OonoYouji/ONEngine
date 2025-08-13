#include "TerrainCollider.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"
#include "Engine/Core/Utility/Utility.h"

TerrainCollider::TerrainCollider() {
	pTerrain_ = nullptr;
	isVertexGenerationRequested_ = true;
}

void TerrainCollider::AttachTerrain() {
	if (IEntity* entity = GetOwner()) {
		pTerrain_ = entity->GetComponent<Terrain>();
	}
}

void TerrainCollider::CopyVertices(DxManager* _dxManager) {
	/// terrainから RWVertices をコピーする
	if (!pTerrain_) {
		return;
	}

	DxResource dxReadbackBuffer;

	{	/// bufferの生成

		D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = sizeof(TerrainVertex) * pTerrain_->GetMaxVertexNum();
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		dxReadbackBuffer.CreateCommittedResource(
			_dxManager->GetDxDevice(), &heapProperties, D3D12_HEAP_FLAG_NONE,
			&bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr
		);
	}

	DxCommand* dxCommand = _dxManager->GetDxCommand();
	auto cmdList = dxCommand->GetCommandList();

	DxResource& dxResource = pTerrain_->GetRwVertices().GetResource();

	dxResource.CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, dxCommand);
	cmdList->CopyResource(dxReadbackBuffer.Get(), dxResource.Get());
	dxResource.CreateBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, dxCommand);

	dxCommand->CommandExecute();
	dxCommand->CommandReset();
	_dxManager->HeapBindToCommandList();

	/// RWVertices をCPUにコピー
	TerrainVertex* gpuData = nullptr;
	dxReadbackBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&gpuData));

	const size_t width = static_cast<size_t>(pTerrain_->GetSize().x);
	const size_t depth = static_cast<size_t>(pTerrain_->GetSize().y);

	vertices_.clear();
	for (size_t z = 0; z < depth; ++z) {
		std::vector<TerrainVertex> row;
		row.reserve(width);
		for (size_t x = 0; x < width; ++x) {
			row.push_back(gpuData[z * width + x]);
		}
		vertices_.push_back(std::move(row));
	}

	dxReadbackBuffer.Get()->Unmap(0, nullptr);

}

float TerrainCollider::GetHeight(const Vector3& _position) {

	/// 条件が満たされない場合は0を返す
	if (!pTerrain_) {
		return 0;
	}

	if (!IsInsideTerrain(_position)) {
		return 0;
	}

	if (vertices_.empty()) {
		return 0;
	}


	/* ----- 座標をuvに変換→indexに変換→頂点座標をworld座標に変換→高さを取得 ----- */

	/// 地形のローカル座標に変換
	const Matrix4x4&& kMatInverse = pTerrain_->GetOwner()->GetTransform()->matWorld.Inverse();
	Vector3 localPosition = Matrix4x4::Transform(_position, kMatInverse);

	/// uv値に変換
	Vector2 uv = Vector2(localPosition.x, localPosition.z) / pTerrain_->GetSize();

	/// indexに変換
	size_t row = static_cast<size_t>(uv.y * pTerrain_->GetSize().y);
	size_t col = static_cast<size_t>(uv.x * pTerrain_->GetSize().x);

	TerrainVertex& vertex = vertices_[row][col];

	/// ローカル座標からワールド座標に変換
	Vector3 vertexPosition = Matrix4x4::Transform(
		Vector3(vertex.position.x, vertex.position.y, vertex.position.z),
		pTerrain_->GetOwner()->GetTransform()->matWorld
	);

	return vertexPosition.y; // 高さを返す
}

Vector3 TerrainCollider::GetGradient(const Vector3& _position) {
	/// 地形のローカル座標に変換
	const Matrix4x4&& kMatInverse = pTerrain_->GetOwner()->GetTransform()->matWorld.Inverse();
	Vector3 localPosition = Matrix4x4::Transform(_position, kMatInverse);

	/// uv値に変換
	Vector2 uv = Vector2(localPosition.x, localPosition.z) / pTerrain_->GetSize();

	/// indexに変換
	size_t row = static_cast<size_t>(uv.y * pTerrain_->GetSize().y);
	size_t col = static_cast<size_t>(uv.x * pTerrain_->GetSize().x);

	// 範囲外ガード
	if (row >= vertices_.size() || col >= vertices_[0].size()) {
		return { 0.0f, 0.0f, 0.0f };
	}

	float h = vertices_[row][col].position.y;

	// X方向の高さ差
	float hL = (col > 0) ? vertices_[row][col - 1].position.y : h;
	float hR = (col < vertices_[0].size() - 1) ? vertices_[row][col + 1].position.y : h;
	float slopeX = std::atan((hR - hL) / (2.0f));

	// Z方向の高さ差
	float hD = (row > 0) ? vertices_[row - 1][col].position.y : h;
	float hU = (row < vertices_.size() - 1) ? vertices_[row + 1][col].position.y : h;
	float slopeZ = std::atan((hU - hD) / (2.0f));

	return { std::abs(slopeX), 0.0f, std::abs(slopeZ) };
}

bool TerrainCollider::IsInsideTerrain(const Vector3& _position) {
	const Matrix4x4&& kMatInverse = pTerrain_->GetOwner()->GetTransform()->matWorld.Inverse();
	Vector3 localPosition = Matrix4x4::Transform(_position, kMatInverse);

	/// 地形のローカル座標上で範囲外にいるかチェック
	if (localPosition.x < 0.0f || localPosition.x > pTerrain_->GetSize().x ||
		localPosition.z < 0.0f || localPosition.z > pTerrain_->GetSize().y) {
		return false;
	}

	return true;
}

Terrain* TerrainCollider::GetTerrain() const {
	return pTerrain_;
}

const std::vector<std::vector<TerrainVertex>>& TerrainCollider::GetVertices() const {
	return vertices_;
}

std::vector<std::vector<TerrainVertex>>& TerrainCollider::GetVertices() {
	return vertices_;
}

bool TerrainCollider::GetIsCreated() const {
	return isCreated_;
}

void TerrainCollider::SetIsVertexGenerationRequested(bool _isRequested) {
	isVertexGenerationRequested_ = _isRequested;
}




void COMP_DEBUG::TerrainColliderDebug(TerrainCollider* _collider) {
	if (!_collider) {
		return;
	}

	if (_collider->GetTerrain()) {
		ImGui::Text("attached terrain");
	} else {
		ImGui::Text("null terrain");
	}


	if (ImGui::Button("attach terrain")) {
		_collider->AttachTerrain();
	}

	if (ImGui::Button("copy vertices")) {
		_collider->SetIsVertexGenerationRequested(true);
	}

}

void from_json(const nlohmann::json& _j, TerrainCollider& _c) {
	_c.enable = _j.value("enable", 1);
}

void to_json(nlohmann::json& _j, const TerrainCollider& _c) {
	_j = {
		{ "type", "TerrainCollider" },
		{ "enable", _c.enable }
	};
}
