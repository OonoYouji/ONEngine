#include "ShadowCaster.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"


void COMP_DEBUG::ShadowCasterDebug(ShadowCaster* _shadowCaster) {
	if (!_shadowCaster) {
		return;
	}


	if (ImGui::Button("toggle camera type")) {
		ShadowCaster* shadowCaster = _shadowCaster;
		CameraComponent* camera = shadowCaster->GetShadowCasterCamera();
		if (camera) {
			int currentType = camera->GetCameraType();
			if (currentType == static_cast<int>(CameraType::Type2D)) {
				camera->SetCameraType(static_cast<int>(CameraType::Type3D));
			} else {
				camera->SetCameraType(static_cast<int>(CameraType::Type2D));
			}
		}
	}


	if (ImGui::Button("create camera")) {
		_shadowCaster->CreateShadowCaster();
	}

}

void from_json(const nlohmann::json& _j, ShadowCaster& _c) {
	_c.enable = _j.value("enable", static_cast<int>(true));
}

void to_json(nlohmann::json& _j, const ShadowCaster& _c) {
	_j = {
		{ "type", "ShadowCaster" },
		{ "enable", _c.enable },
	};
}


/// ///////////////////////////////////////////////////
/// 影の投影を行うためのコンポーネント
/// ///////////////////////////////////////////////////

ShadowCaster::ShadowCaster()
	: camera_(nullptr),
	isCreated_(false) {
};

ShadowCaster::~ShadowCaster() = default;

void ShadowCaster::CreateShadowCaster() {
	/// 生成済みかチェック
	if (!isCreated_) {
		isCreated_ = true;
	} else {
		Console::LogWarning("ShadowCaster::CreateShadowCaster: ShadowCaster is already created.");
		return;
	}

	camera_ = GetOwner()->AddComponent<CameraComponent>();
	camera_->SetCameraType(static_cast<int>(CameraType::Type2D));
	camera_->SetIsMainCameraRequest(false); /// シャドウキャスター用カメラはメインカメラにしない
}

void ShadowCaster::CalculationLightViewMatrix(DirectionalLight* _directionLight) {
	/// nullチェック
	if (!_directionLight) {
		Console::LogError("ShadowCaster::CalculationLightViewMatrix: DirectionalLight is null");
		return;
	}


	/// オーナーの位置を取得し、ライトの方向を基にカメラの回転を計算
	GameEntity* owner = GetOwner();
	Vector3 lightPos = owner->GetPosition();

	const Vector3& dir = _directionLight->GetDirection();
	Quaternion cameraRotation = Quaternion::LookAt(
		{}, dir,
		Vector3::kUp
	);

	/// カメラの回転を設定
	owner->SetRotate(cameraRotation);
	camera_->UpdateViewProjection();

}

CameraComponent* ShadowCaster::GetShadowCasterCamera() {
	return camera_;
}

