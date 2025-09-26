#include "CameraUpdateSystem.h"

/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

CameraUpdateSystem::CameraUpdateSystem(DxDevice* _dxDevice) : pDxDevice_(_dxDevice) {
	pMainCamera_ = nullptr;
}

void CameraUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
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
		if (cameraComponent->GetIsMainCameraRequest()) {

			int type = cameraComponent->GetCameraType();
			if (type == static_cast<int>(CameraType::Type3D)) {
				if (pMainCamera_ != cameraComponent) {
					/// 古い方をfalseに戻す
					if (pMainCamera_ && pMainCamera_->cameraType_ == static_cast<int>(CameraType::Type3D)) {
						pMainCamera_->SetIsMainCameraRequest(false);
					}
					pMainCamera_ = cameraComponent; ///< main cameraを設定
				}
			} else if (type == static_cast<int>(CameraType::Type2D)) {
				if (pMainCamera2D_ != cameraComponent) {
					/// 古い方をfalseに戻す
					if (pMainCamera2D_ && pMainCamera_->cameraType_ == static_cast<int>(CameraType::Type2D)) {
						pMainCamera2D_->SetIsMainCameraRequest(false);
					}
					pMainCamera2D_ = cameraComponent; ///< main camera 2Dを設定
				}
			}

		}
	}

	/// ecsにmain cameraを設定
	_ecs->SetMainCamera(pMainCamera_);
	_ecs->SetMainCamera2D(pMainCamera2D_);

}

void CameraUpdateSystem::RuntimeUpdate(ECSGroup*) {

}
