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


	if (ImGui::Button("create camera")) {
		_shadowCaster->CreateShadowCaster(nullptr);
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

ShadowCaster::ShadowCaster() : camera_(nullptr) {};
ShadowCaster::~ShadowCaster() = default;

void ShadowCaster::CreateShadowCaster(DxDevice* _dxDevice) {
	camera_ = GetOwner()->AddComponent<CameraComponent>();
	camera_->SetCameraType(static_cast<int>(CameraType::Type2D));

	//vpBuffer_.Create(_dxDevice);


	///// DirectionalLightからライトビュー行列を計算
	//if (ECSGroup* ecsGroup = GetOwner()->GetECSGroup()) {
	//	ComponentArray<DirectionalLight>* dirLightArray = ecsGroup->GetComponentArray<DirectionalLight>();
	//	if (dirLightArray && !dirLightArray->GetUsedComponents().empty()) {
	//		DirectionalLight* dirLight = dirLightArray->GetUsedComponents().front();

	//		CalculationLightViewMatrix(dirLight);
	//	}
	//}
}

void ShadowCaster::CreateVPBuffer(DxDevice* _dxDevice) {
	//vpBuffer_.Create(_dxDevice);
}

void ShadowCaster::CalculationLightViewMatrix(DirectionalLight* _directionLight) {
	/// nullチェック
	if (!_directionLight) {
		Console::LogError("ShadowCaster::CalculationLightViewMatrix: DirectionalLight is null");
		return;
	}


	//const Vector3& lightDir = _directionLight->GetDirection();
	//const Vector3& upDir = Vector3::kUp;

	//GameEntity* owner = GetOwner();
	//const Vector3& lightPos = owner->GetPosition();

	///// 行列の計算
	//matLightView_ = Matrix4x4::MakeLookAtLH(lightPos, lightPos + lightDir, upDir);
	//matLightProj_ = CameraMath::MakeOrthographicMatrix(
	//	-(EngineConfig::kWindowSize.x / 2.0f), (EngineConfig::kWindowSize.x / 2.0f),
	//	-(EngineConfig::kWindowSize.y / 2.0f), (EngineConfig::kWindowSize.y / 2.0f),
	//	nearClip_, farClip_
	//);


	//vpBuffer_.SetMappedData(ViewProjection(
	//	matLightView_ * matLightProj_,
	//	matLightView_, matLightProj_
	//));

}

CameraComponent* ShadowCaster::GetShadowCasterCamera() {
	return camera_;
}

