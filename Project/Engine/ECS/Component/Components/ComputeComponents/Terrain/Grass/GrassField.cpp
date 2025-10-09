#include "GrassField.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"

/// ////////////////////////////////////////////////////////
/// Json Serialization
/// ////////////////////////////////////////////////////////

void to_json(nlohmann::json& _j, const GrassField& _p) {
	/// GrassField -> Json
	_j = {
		{ "type", "GrassField" },
		{ "maxGrassCount", _p.maxGrassCount_ },
		{ "distributionTexturePath", _p.distributionTexturePath_ },
	};
}

void from_json(const nlohmann::json& _j, GrassField& _p) {
	/// Json -> GrassField
	_p.maxGrassCount_ = _j.value("maxGrassCount", 128);
	_p.distributionTexturePath_ = _j.value("distributionTexturePath", "");
}


/// ////////////////////////////////////////////////////////
/// ImGuiデバッグ関数
/// ////////////////////////////////////////////////////////

void COMP_DEBUG::GrassFieldDebug(GrassField* _grassField) {

	/// 草の最大本数
	ImGui::Text("Max Blade Count : %d", _grassField->GetMaxGrassCount());

	/// 配置に使うテクスチャのパス
	ImGui::Text("Distribution Texture Path : %s", _grassField->distributionTexturePath_.c_str());

	/// 配置対象のTerrainComponentのDrag&Drop
	ImGui::Text("Drag & Drop Terrain Component here");
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TerrainComponent")) {
			const uintptr_t terrainCompPtr = *(const uintptr_t*)payload->Data;
			// ここでterrainCompPtrを使って必要な処理を行う
			// 例: _grassField->SetTerrainComponent(reinterpret_cast<TerrainComponent*>(terrainCompPtr));
		}
		ImGui::EndDragDropTarget();
	}

}


/// ////////////////////////////////////////////////////////
/// GrassField
/// ////////////////////////////////////////////////////////

GrassField::GrassField() : maxGrassCount_(128), distributionTexturePath_("") {};
GrassField::~GrassField() = default;

void GrassField::Initialize(uint32_t _maxBladeCount, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {
	maxGrassCount_ = _maxBladeCount;
	/// 草のインスタンスバッファの作成
	rwGrassInstanceBuffer_.CreateUAV(
		maxGrassCount_, _dxDevice, _dxCommand, _dxSRVHeap
	);
}

StructuredBuffer<GrassInstance>& GrassField::GetRwGrassInstanceBuffer() {
	return rwGrassInstanceBuffer_;
}

uint32_t GrassField::GetMaxGrassCount() const {
	return maxGrassCount_;
}

