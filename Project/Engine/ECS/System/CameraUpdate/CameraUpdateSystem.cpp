#include "CameraUpdateSystem.h"

/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

CameraUpdateSystem::CameraUpdateSystem(DxDevice* _dxDevice) : pDxDevice_(_dxDevice) {}

void CameraUpdateSystem::Update(EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) {
	/// カメラのcomponentを集める

	ComponentArray<CameraComponent>* cameraArray = _ecs->GetComponentArray<CameraComponent>();
	if (!cameraArray) {
		return; /// カメラのコンポーネント配列が存在しない場合は何もしない
	}

	auto& cameraComponents = cameraArray->GetUsedComponents();
	for (auto& cameraComponent : cameraComponents) {
		if (!cameraComponent) {
			continue; /// nullptrチェック
		}

		/// ViewProjectionが作られていなければ作成する
		if (!cameraComponent->IsMakeViewProjection()) {
			cameraComponent->MakeViewProjection(pDxDevice_);
		}

		/// カメラのViewProjectionを更新
		cameraComponent->UpdateViewProjection();

		/// main camera かどうか
		if (cameraComponent->GetIsMainCamera()) {
			/// 新しくmain cameraに設定された
			if (pMainCamera_ != cameraComponent) {
				/// 古い方をfalseに戻す
				if (pMainCamera_) {
					pMainCamera_->SetIsMainCamera(false);
				}

				pMainCamera_ = cameraComponent; ///< main cameraを設定
			}
		}
	}

	/// ecsにmain cameraを設定

}
