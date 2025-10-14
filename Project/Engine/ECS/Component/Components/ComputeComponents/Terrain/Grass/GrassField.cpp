#include "GrassField.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

/// ////////////////////////////////////////////////////////
/// Json Serialization
/// ////////////////////////////////////////////////////////

void to_json(nlohmann::json& _j, const GrassField& _p) {
	/// GrassField -> Json
	_j = {
		{ "type", "GrassField" },
		{ "maxGrassCount", _p.maxGrassCount_ },
		{ "distributionTexturePath", _p.distributionTexturePath_ },
		{ "material", _p.material_ }
	};
}

void from_json(const nlohmann::json& _j, GrassField& _p) {
	/// Json -> GrassField
	_p.maxGrassCount_ = _j.value("maxGrassCount", 128);
	_p.distributionTexturePath_ = _j.value("distributionTexturePath", "");
	_p.material_ = _j.value("material", Material{});
}


/// ////////////////////////////////////////////////////////
/// ImGuiデバッグ関数
/// ////////////////////////////////////////////////////////

void COMP_DEBUG::GrassFieldDebug(GrassField* _grassField, GraphicsResourceCollection* _grc) {

	/// 草の最大本数
	ImGui::Text("Max Blade Count : %d", _grassField->GetMaxGrassCount());

	/// 配置に使うテクスチャのパス
	ImGui::Text("Distribution Texture Path : %s", _grassField->distributionTexturePath_.c_str());

	/// 配置対象のTerrainComponentのDrag&Drop
	ImGui::CollapsingHeader("Drag & Drop Terrain Component here");
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Component")) {
			/// payloadからコンポーネントを取得
			IComponent* component = *(IComponent**)payload->Data;
			if (component) {
				/// TerrainComponentかどうかをチェック
				if (Terrain* terrain = dynamic_cast<Terrain*>(component)) {
					Console::Log(std::format("GrassField : Drag & Drop Terrain Component (id : {})", terrain->id));
				}
			}

		}


		ImGui::EndDragDropTarget();
	}


	/// material debug
	ImMathf::MaterialEdit("material", &_grassField->material_, _grc);

}


/// ////////////////////////////////////////////////////////
/// GrassField
/// ////////////////////////////////////////////////////////

GrassField::GrassField() :
	maxGrassCount_(static_cast<uint32_t>(std::pow(2, 32) - 1)), distributionTexturePath_(""), isCreated_(false), isArranged_(false), material_({}) {
};
GrassField::~GrassField() = default;

void GrassField::Initialize(uint32_t _maxBladeCount, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {

	/// すでに生成されていたら何もしない
	if (isCreated_) {
		return;
	} else {
		isCreated_ = true;
	}

	maxGrassCount_ = _maxBladeCount;
	/// 草のインスタンスバッファの作成
	rwGrassInstanceBuffer_.CreateSRVAndUAV(
		maxGrassCount_, _dxDevice, _dxCommand, _dxSRVHeap
	);

	timeBuffer_.CreateUAV(maxGrassCount_, _dxDevice, _dxCommand, _dxSRVHeap);
	materialBuffer_.Create(_dxDevice);
}

void GrassField::MaterialMapping() {
	materialBuffer_.SetMappedData(material_);
}

StructuredBuffer<GrassInstance>& GrassField::GetRwGrassInstanceBuffer() {
	return rwGrassInstanceBuffer_;
}

StructuredBuffer<float>& GrassField::GetTimeBuffer() {
	return timeBuffer_;
}

ConstantBuffer<Material>& GrassField::GetMaterialBufferRef() {
	return materialBuffer_;
}

uint32_t GrassField::GetMaxGrassCount() const {
	return maxGrassCount_;
}

bool GrassField::GetIsCreated() const {
	return isCreated_;
}

