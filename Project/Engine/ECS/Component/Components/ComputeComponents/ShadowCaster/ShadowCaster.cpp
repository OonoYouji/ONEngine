#include "ShadowCaster.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"


namespace {

	Vector2 CalcAspectSize(float _aspectRatio, const Vector2& _size) {
		Vector2 result{};
		if (_size.x > 0.0f) {
			result.x = _size.x;
			result.y = _size.y / _aspectRatio;
		} else if (_size.y > 0.0f) {
			result.y = _size.y;
			result.x = result.y * _aspectRatio;
		} else {
			result = EngineConfig::kWindowSize;
		}

		return result;
	}

}


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


	ImGui::SeparatorText("light parameter");

	ImMathf::DragFloat3("base light pos", &_shadowCaster->baseLightPos_, 1.0f, -10000.0f, 10000.0f);
	ImMathf::DragFloat("light length", &_shadowCaster->lightLength_, 1.0f, 0.0f, 10000.0f);


	ImGui::SeparatorText("shadow caster parameters");

	ImMathf::DragFloat2("window size", &_shadowCaster->orthographicSize_, 1.0f, 0.0f, 0.0f);
	ImMathf::DragFloat("scale factor", &_shadowCaster->scaleFactor_, 0.01f, 0.0f, 10.0f);


	ImGui::Spacing();

	ImMathf::DragFloat2("texel size", &_shadowCaster->texelSizeShadow_, 0.01f, 0.0f, 0.0f);
	ImMathf::DragFloat("shadow bias", &_shadowCaster->shadowBias_, 0.001f, 0.0f, 1.0f);
	ImMathf::DragFloat("shadow darkness", &_shadowCaster->shadowDarkness_, 0.01f, 0.0f, 1.0f);
	ImMathf::DragInt("pcf radius", &_shadowCaster->pcfRadius_, 1, 0, 10);

}

void from_json(const nlohmann::json& _j, ShadowCaster& _c) {
	_c.enable = _j.value("enable", static_cast<int>(true));

	_c.orthographicSize_ = _j.value("orthographicSize", EngineConfig::kWindowSize);
	_c.scaleFactor_ = _j.value("scaleFactor", 1.0f);
	_c.texelSizeShadow_ = _j.value("texelSizeShadow", Vector2(1.0f / EngineConfig::kWindowSize.x, 1.0f / EngineConfig::kWindowSize.y));
	_c.shadowBias_ = _j.value("shadowBias", 0.005f);
	_c.shadowDarkness_ = _j.value("shadowDarkness", 0.7f);
	_c.pcfRadius_ = _j.value("pcfRadius", 2);
	_c.baseLightPos_ = _j.value("baseLightPos", Vector3(500, 0, 500));
	_c.lightLength_ = _j.value("lightLength", 300.0f);

}

void to_json(nlohmann::json& _j, const ShadowCaster& _c) {
	_j = {
		{ "type", "ShadowCaster" },
		{ "enable", _c.enable },
		{ "orthographicSize", _c.orthographicSize_ },
		{ "scaleFactor", _c.scaleFactor_ },
		{ "texelSizeShadow", _c.texelSizeShadow_ },
		{ "shadowBias", _c.shadowBias_ },
		{ "shadowDarkness", _c.shadowDarkness_ },
		{ "pcfRadius", _c.pcfRadius_ },
		{ "baseLightPos", _c.baseLightPos_ },
		{ "lightLength", _c.lightLength_ }
	};
}


/// ///////////////////////////////////////////////////
/// 影の投影を行うためのコンポーネント
/// ///////////////////////////////////////////////////

ShadowCaster::ShadowCaster()
	: camera_(nullptr),
	isCreated_(false),
	orthographicSize_(EngineConfig::kWindowSize),
	texelSizeShadow_(Vector2(1.0f / EngineConfig::kWindowSize.x, 1.0f / EngineConfig::kWindowSize.y)),
	shadowBias_(0.005f),
	shadowDarkness_(0.7f),
	pcfRadius_(2),
	baseLightPos_(Vector3(500, 0, 500)),
	lightLength_(300) {

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
	camera_->SetOrthographicSize(Vector2(1280.0f, 720.0f) * 0.01f);
}

void ShadowCaster::CalculationLightViewMatrix(ECSGroup* _ecsGroup, DirectionalLight* _directionLight) {
	/// nullチェック
	if (!_directionLight) {
		Console::LogError("ShadowCaster::CalculationLightViewMatrix: DirectionalLight is null");
		return;
	}


	/// オーナーの位置を取得し、ライトの方向を基にカメラの回転を計算
	GameEntity* owner = GetOwner();

	const Vector3& dir = _directionLight->GetDirection();
	Quaternion cameraRotation = Quaternion::LookAt(
		{}, dir,
		Vector3::kUp
	);

	/// カメラの回転を設定
	owner->SetRotate(cameraRotation);

	/// 現在のグループのmain cameraを取得し、座標を補正
	CameraComponent* mainCamera = _ecsGroup->GetMainCamera();
	GameEntity* mainCamEntity = mainCamera->GetOwner();
	mainCamEntity->UpdateTransform();
	Vector3 mainCamPos = mainCamEntity->GetPosition();

	/// ライトの方向に合わせて座標を計算
	owner->SetPosition(mainCamPos + baseLightPos_ - dir * lightLength_);

	camera_->SetOrthographicSize(orthographicSize_ * scaleFactor_);
	camera_->UpdateViewProjection();

}

CameraComponent* ShadowCaster::GetShadowCasterCamera() {
	return camera_;
}

ShadowParameter ShadowCaster::GetShadowParameters() const {
	return ShadowParameter{
		.screenSize = EngineConfig::kWindowSize,
		.texelSizeShadow = texelSizeShadow_,
		.shadowBias = shadowBias_,
		.shadowDarkness = shadowDarkness_,
		.pcfRadius = pcfRadius_,
	};
}

