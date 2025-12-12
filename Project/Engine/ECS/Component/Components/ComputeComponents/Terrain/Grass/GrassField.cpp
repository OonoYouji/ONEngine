#include "GrassField.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Asset/Collection/AssetCollection.h"

/// editor
#include "Engine/Editor/Math/AssetDebugger.h"
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;

/// ////////////////////////////////////////////////////////
/// Json Serialization
/// ////////////////////////////////////////////////////////

void ONEngine::to_json(nlohmann::json& _j, const GrassField& _p) {
	/// GrassField -> Json
	_j = {
		{ "type", "GrassField" },
		{ "maxGrassCount", _p.maxGrassCount_ },
		{ "distributionTexturePath", _p.distributionTexturePath_ },
		{ "material", _p.material_ }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, GrassField& _p) {
	/// Json -> GrassField
	_p.maxGrassCount_ = _j.value("maxGrassCount", 128);
	_p.distributionTexturePath_ = _j.value("distributionTexturePath", "");
	_p.material_ = _j.value("material", Material{});
}


/// ////////////////////////////////////////////////////////
/// ImGuiデバッグ関数
/// ////////////////////////////////////////////////////////

void ComponentDebug::GrassFieldDebug(GrassField* _grassField, AssetCollection* _assetCollection) {

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
	Editor::ImMathf::MaterialEdit("material", &_grassField->material_, _assetCollection);

}


/// ////////////////////////////////////////////////////////
/// GrassField
/// ////////////////////////////////////////////////////////

GrassField::GrassField() :
	maxGrassCount_(static_cast<uint32_t>(std::pow(2, 32) - 1)), distributionTexturePath_(""), isCreated_(false), isArranged_(false) {
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
	rwGrassInstanceBuffer_.CreateAppendBuffer(
		maxGrassCount_, _dxDevice, _dxCommand, _dxSRVHeap
	);

	startIndexBuffer_.Create(2000, _dxDevice, _dxSRVHeap);

	timeBuffer_.CreateUAV(maxGrassCount_, _dxDevice, _dxCommand, _dxSRVHeap);
	materialBuffer_.Create(_dxDevice);
}

void GrassField::SetupRenderingData(AssetCollection* _assetCollection) {

	GPUMaterial gpuMaterial{};

	/// material_の情報をgpuMaterial_にセット
	gpuMaterial.baseColor = material_.baseColor;
	gpuMaterial.postEffectFlags = material_.postEffectFlags;
	gpuMaterial.uvTransform = material_.uvTransform;
	gpuMaterial.entityId = GetOwner()->GetId();

	/// テクスチャの情報をセット
	if (material_.HasBaseTexture()) {
		const Guid& baseTextureGuid = material_.GetBaseTextureGuid();
		gpuMaterial.baseTextureId = static_cast<int32_t>(_assetCollection->GetTextureIndexFromGuid(baseTextureGuid));
	} else {
		gpuMaterial.baseTextureId = 0;
	}

	if (material_.HasNormalTexture()) {
		const Guid& normalTextureGuid = material_.GetNormalTextureGuid();
		gpuMaterial.normalTextureId = static_cast<int32_t>(_assetCollection->GetTextureIndexFromGuid(normalTextureGuid));
	} else {
		gpuMaterial.normalTextureId = 0;
	}

	materialBuffer_.SetMappedData(gpuMaterial);
}

void GrassField::StartIndexMapping(UINT _oneDrawInstanceCount) {
	//
	
	UINT forLoopCount = (maxGrassCount_ + _oneDrawInstanceCount - 1) / _oneDrawInstanceCount;
	for(UINT i = 0; i < forLoopCount; i++) {
		uint32_t mappedData = i * _oneDrawInstanceCount;
		startIndexBuffer_.SetMappedData(i, mappedData);
	}
	
}

void GrassField::AppendBufferReadCounter(DxManager* _dxm, DxCommand* _dxCommand) {
	/// ----- GrassInstanceBufferのカウンターを呼んでインスタンス数を数える ----- ///

	auto cmdList = _dxCommand->GetCommandList();

	D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(rwGrassInstanceBuffer_.GetResource().Get());
	cmdList->ResourceBarrier(1, &uavBarrier);
	rwGrassInstanceBuffer_.GetCounterResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);

	_dxCommand->CommandExecuteAndWait();
	_dxCommand->CommandReset();
	_dxCommand->WaitForGpuComplete();

	instanceCount_ = rwGrassInstanceBuffer_.ReadCounter(_dxCommand);

	_dxm->HeapBindToCommandList();

	/// 配置は一回しか行わないのでカウンターのリセットはしない(今後複数回配置するようになったらリセットする)

}

StructuredBuffer<GrassData>& GrassField::GetRwGrassInstanceBuffer() {
	return rwGrassInstanceBuffer_;
}

StructuredBuffer<uint32_t>& GrassField::GetStartIndexBufferRef() {
	return startIndexBuffer_;
}

StructuredBuffer<float>& GrassField::GetTimeBuffer() {
	return timeBuffer_;
}

ConstantBuffer<GPUMaterial>& GrassField::GetMaterialBufferRef() {
	return materialBuffer_;
}

uint32_t GrassField::GetMaxGrassCount() const {
	return maxGrassCount_;
}

bool GrassField::GetIsCreated() const {
	return isCreated_;
}

uint32_t GrassField::GetInstanceCount() const {
	return instanceCount_;
}
