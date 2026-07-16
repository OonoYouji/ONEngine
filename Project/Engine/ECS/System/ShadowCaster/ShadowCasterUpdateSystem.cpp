#include "ShadowCasterUpdateSystem.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"

/**
 * @brief コンストラクタ
 */
ShadowCasterUpdateSystem::ShadowCasterUpdateSystem() = default;

/**
 * @brief デストラクタ
 */
ShadowCasterUpdateSystem::~ShadowCasterUpdateSystem() = default;

/**
 * @brief エディタ非実行時のシャドウキャスター情報の更新処理を行います。
 */
void ShadowCasterUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	if (!DebugConfig::isDebugging) {
		Update(_ecs);
	}

}

/**
 * @brief ランタイム実行時のシャドウキャスター情報の更新処理を実行します。
 */
void ShadowCasterUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
	if (DebugConfig::isDebugging) {
		Update(_ecs);
	}
}

/**
 * @brief 指定されたECSグループ内の全シャドウキャスターのビュープロジェクション同期と定数バッファ同期を行う共通更新処理です。
 */
void ShadowCasterUpdateSystem::Update(ECSGroup* _ecs) {
	/// ----- ShadowCasterの更新 ----- ///


	/// ShadowCasterの配列を取得&空ではないかチェック
	ComponentArray<ShadowCaster>* shadowCasterArray = _ecs->GetComponentArray<ShadowCaster>();
	if (!shadowCasterArray || shadowCasterArray->GetUsedComponents().empty()) {
		return;
	}


	/// DirectionalLightの配列を取得&空ではないかチェック
	ComponentArray<DirectionalLight>* dirLightArray = _ecs->GetComponentArray<DirectionalLight>();
	DirectionalLight* dirLight = nullptr;
	if (dirLightArray && !dirLightArray->GetUsedComponents().empty()) {
		dirLight = dirLightArray->GetUsedComponents().front();
	}

	for (auto& shadowCaster : shadowCasterArray->GetUsedComponents()) {
		shadowCaster->CreateShadowCaster();

		/// DirectionalLightが存在する場合、ライトビュー行列を計算
		if (dirLight) {
			shadowCaster->CalculationLightViewMatrix(_ecs, dirLight);
		}
	}
}
