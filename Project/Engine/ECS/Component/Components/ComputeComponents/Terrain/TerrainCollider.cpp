#include "TerrainCollider.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"

TerrainCollider::TerrainCollider() {
	pTerrain_ = nullptr;
	isVertexGenerationRequested_ = false;
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

	DxResource& dxResource = pTerrain_->GetRwVertices().GetResource();

	DxCommand* dxCommand = _dxManager->GetDxCommand();
	auto cmdList = dxCommand->GetCommandList();
	cmdList->CopyResource(dxReadbackBuffer.Get(), dxResource.Get());

	dxCommand->CommandExecute();
	dxCommand->CommandReset();
	_dxManager->HeapBindToCommandList();

	/// RWVertices をCPUにコピー
	TerrainVertex* gpuData = nullptr;
	dxReadbackBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&gpuData));

	const size_t width = static_cast<size_t>(pTerrain_->GetSize().x);
	const size_t depth = static_cast<size_t>(pTerrain_->GetSize().y);

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

Terrain* TerrainCollider::GetTerrain() const {
	return pTerrain_;
}

const std::vector<std::vector<TerrainVertex>>& TerrainCollider::GetVertices() const {
	return vertices_;
}

std::vector<std::vector<TerrainVertex>>& TerrainCollider::GetVertices() {
	return vertices_;
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
