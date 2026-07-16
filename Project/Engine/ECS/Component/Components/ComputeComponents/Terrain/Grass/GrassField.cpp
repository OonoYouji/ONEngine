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

/**
 * @brief JSONへのシリアライズ
 */
void ONEngine::to_json(nlohmann::json& _j, const GrassField& _p) {
	/// GrassField -> Json
	_j = {
		{ "type", "GrassField" },
		{ "maxGrassCount", _p.maxGrassCount_ },
		{ "distributionTexturePath", _p.distributionTexturePath_ },
		{ "material", _p.material_ }
	};
}

/**
 * @brief JSONからのデシリアライズ
 */
void ONEngine::from_json(const nlohmann::json& _j, GrassField& _p) {
	/// Json -> GrassField
	_p.maxGrassCount_ = _j.value("maxGrassCount", 128);
	_p.distributionTexturePath_ = _j.value("distributionTexturePath", "");

	_p.material_ = _j.value("material", Asset::Material{});
}


/// ////////////////////////////////////////////////////////
/// ImGuiデバッグ関数
/// ////////////////////////////////////////////////////////

/**
 * @brief エディタ用：GrassFieldコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::GrassFieldDebug(GrassField* _grassField, Asset::AssetCollection* _assetCollection) {

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

/**
 * @brief コンストラクタ
 */
GrassField::GrassField() :
	maxGrassCount_(static_cast<uint32_t>(std::pow(2, 32) - 1)), distributionTexturePath_(""), isCreated_(false), isArranged_(false) {
};
/**
 * @brief デストラクタ
 */
GrassField::~GrassField() = default;

/**
 * @brief 最大草ブレード数に対応するGPUバッファ（UAV構造化バッファ等）を構築・初期化します。
 */
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

/**
 * @brief マテリアル定数バッファ等の描画に必要な定数リソースデータをセットアップ（GPUへ転送）します。
 */
void GrassField::SetupRenderingData(Asset::AssetCollection* _assetCollection) {

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

/**
 * @brief 描画コマンド用の開始インデックスバッファパラメータを設定・構築します。
 */
void GrassField::StartIndexMapping(UINT _oneDrawInstanceCount) {
	//

	UINT forLoopCount = (maxGrassCount_ + _oneDrawInstanceCount - 1) / _oneDrawInstanceCount;
	for (UINT i = 0; i < forLoopCount; i++) {
		uint32_t mappedData = i * _oneDrawInstanceCount;
		startIndexBuffer_.SetMappedData(i, mappedData);
	}

}

/**
 * @brief 配置完了した有効な草ブレードインスタンス数を調べるカウンタバッファから、CPU側に本数を読み戻します。
 */
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

/**
 * @brief 草インスタンスデータ用の構造化バッファオブジェクトへの参照を取得します。
 */
StructuredBuffer<GrassData>& GrassField::GetRwGrassInstanceBuffer() {
	return rwGrassInstanceBuffer_;
}

/**
 * @brief 描画コマンド用の開始インデックス構造化バッファを取得します。
 */
StructuredBuffer<uint32_t>& GrassField::GetStartIndexBufferRef() {
	return startIndexBuffer_;
}

/**
 * @brief 風アニメーション等に使う時間情報を転送する構造化バッファを取得します。
 */
StructuredBuffer<float>& GrassField::GetTimeBuffer() {
	return timeBuffer_;
}

/**
 * @brief マテリアルパラメータ（GPUMaterial）の定数バッファを取得します。
 */
ConstantBuffer<GPUMaterial>& GrassField::GetMaterialBufferRef() {
	return materialBuffer_;
}

/**
 * @brief 最大配置可能草ブレード数を取得します。
 */
uint32_t GrassField::GetMaxGrassCount() const {
	return maxGrassCount_;
}

/**
 * @brief バッファ初期構築完了フラグを取得します。
 */
bool GrassField::GetIsCreated() const {
	return isCreated_;
}

/**
 * @brief 実際に配置完了した草ブレードインスタンス数を取得します。
 */
uint32_t GrassField::GetInstanceCount() const {
	return instanceCount_;
}
