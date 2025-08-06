#include "TerrainCollider.h"

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

TerrainCollider::TerrainCollider() {
	pTerrain_ = nullptr;
	//if (IEntity* entity = GetOwner()) {
	//	pTerrain_ = entity->GetComponent<Terrain>();
	//}
}

void TerrainCollider::CopyVertices() {
	/// terrainから RWVertices をコピーする
	if (!pTerrain_) {
		return;
	}

	/// RWVertices をCPUにコピー
	TerrainVertex* gpuData = nullptr;
	DxResource& dxResource = pTerrain_->GetRwVertices().GetResource();
	dxResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&gpuData));

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

	dxResource.Get()->Unmap(0, nullptr);

}




void COMP_DEBUG::TerrainColliderDebug(TerrainCollider* _collider) {
	if (_collider) {
		return;
	}

}

void from_json(const nlohmann::json& _j, TerrainCollider& _c) {
	_c.enable = _j.value("enable", true);
}

void to_json(nlohmann::json& _j, const TerrainCollider& _c) {
	_j = {
		{ "type", "TerrainCollider" },
		{ "enable", _c.enable }
	};
}
