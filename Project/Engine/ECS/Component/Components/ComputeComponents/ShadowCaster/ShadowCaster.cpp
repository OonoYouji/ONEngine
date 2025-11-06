#include "ShadowCaster.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


ShadowCaster::ShadowCaster() = default;
ShadowCaster::~ShadowCaster() = default;

void ShadowCaster::CreateVPBuffer(DxDevice* _dxDevice) {
	vpBuffer_.Create(_dxDevice);
}

void ShadowCaster::CalculationLightViewMatrix(DirectionalLight* _directionLight) {
	const Vector3& lightDir = _directionLight->GetDirection();
	const Vector3& upDir = Vector3::kUp;

	GameEntity* owner = GetOwner();
	const Vector3& lightPos = owner->GetPosition();

	/// 行列の計算
	matLightView_ = Matrix4x4::MakeLookAtLH(lightPos, lightPos + lightDir, upDir);
	matLightProj_ = CameraMath::MakeOrthographicMatrix(
		-(EngineConfig::kWindowSize.x / 2.0f), (EngineConfig::kWindowSize.x / 2.0f),
		-(EngineConfig::kWindowSize.y / 2.0f), (EngineConfig::kWindowSize.y / 2.0f),
		nearClip_, farClip_
	);


	vpBuffer_.SetMappedData(ViewProjection(
		matLightView_ * matLightProj_,
		matLightView_, matLightProj_
	));

}
