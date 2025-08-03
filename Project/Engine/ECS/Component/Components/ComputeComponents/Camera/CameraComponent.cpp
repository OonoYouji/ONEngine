#include "CameraComponent.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Math/Mathf.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"



CameraComponent::CameraComponent() {
	/// デフォルト値を設定
	SetFovY(0.7f);
	SetNearClip(0.1f);
	SetFarClip(1000.0f);
	SetIsMainCamera(false);
	SetCameraType(static_cast<int>(CameraType::Type3D));
}
CameraComponent::~CameraComponent() {}

void CameraComponent::UpdateViewProjection() {
	IEntity* entity = GetOwner();
	if (!entity) {
		return; // エンティティが存在しない場合は何もしない
	}

	entity->UpdateTransform(); /// transformの更新し忘れ防止
	matView_ = entity->GetTransform()->GetMatWorld().Inverse();

	if (cameraType_ == static_cast<int>(CameraType::Type3D)) {
		/// 3Dカメラの場合
		matProjection_ = CameraMath::MakePerspectiveFovMatrix(
			fovY_, EngineConfig::kWindowSize.x / EngineConfig::kWindowSize.y,
			nearClip_, farClip_
		);

	} else {
		/// 2Dカメラの場合

	}

	viewProjection_.SetMappedData(ViewProjection(matView_ * matProjection_));

}

void CameraComponent::MakeViewProjection(DxDevice* _dxDevice) {
	viewProjection_.Create(_dxDevice);
	viewProjection_.SetMappedData(ViewProjection(
		Matrix4x4::kIdentity
	));
}


Matrix4x4 CameraMath::MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip) {
	return Matrix4x4(
		(1 / _aspectRatio) * Mathf::Cot(_fovY / 2.0f), 0.0f, 0.0f, 0.0f,
		0.0f, Mathf::Cot(_fovY / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, _farClip / (_farClip - _nearClip), 1.0f,
		0.0f, 0.0f, (-_nearClip * _farClip) / (_farClip - _nearClip), 0.0f
	);
}

Matrix4x4 CameraMath::MakeOrthographicMatrix(float _left, float _right, float _bottom, float _top, float _znear, float _zfar) {
	Matrix4x4 result = {};

	float width = _right - _left;
	float height = _top - _bottom;
	float depth = _zfar - _znear;

	result.m[0][0] = 2.0f / width;
	result.m[1][1] = 2.0f / height;
	result.m[2][2] = 1.0f / depth;
	result.m[3][0] = -(_right + _left) / width;
	result.m[3][1] = -(_top + _bottom) / height;
	result.m[3][2] = -_znear / depth;
	result.m[3][3] = 1.0f;

	return result;
}

void CameraComponent::SetIsMainCamera(bool _isMainCamera) {
	isMainCamera_ = _isMainCamera;
}

void CameraComponent::SetFovY(float _fovY) {
	fovY_ = _fovY;
}

void CameraComponent::SetNearClip(float _nearClip) {
	nearClip_ = _nearClip;
}

void CameraComponent::SetFarClip(float _farClip) {
	farClip_ = _farClip;
}

void CameraComponent::SetCameraType(int _cameraType) {
	cameraType_ = _cameraType;
}

bool CameraComponent::GetIsMainCamera() const {
	return isMainCamera_;
}

float CameraComponent::GetFovY() const {
	return fovY_;
}

float CameraComponent::GetNearClip() const {
	return nearClip_;
}

float CameraComponent::GetFarClip() const {
	return farClip_;
}

int CameraComponent::GetCameraType() const {
	return cameraType_;
}

bool CameraComponent::IsMakeViewProjection() const {
	return viewProjection_.Get() != nullptr;
}

const ViewProjection& CameraComponent::GetViewProjection() const {
	return viewProjection_.GetMappingData();
}

ConstantBuffer<ViewProjection>& CameraComponent::GetViewProjectionBuffer() {
	return viewProjection_;
}

const Matrix4x4& CameraComponent::GetViewMatrix() const {
	return matView_;
}

const Matrix4x4& CameraComponent::GetProjectionMatrix() const {
	return matProjection_;
}



void from_json(const nlohmann::json& _j, CameraComponent& _c) {
	_c.SetIsMainCamera(_j.value("isMainCamera", false));
	_c.SetFovY(_j.value("fovY", 0.7f));
	_c.SetNearClip(_j.value("nearClip", 0.1f));
	_c.SetFarClip(_j.value("farClip", 1000.0f));
}

void to_json(nlohmann::json& _j, const CameraComponent& _c) {
	_j = nlohmann::json{
		{ "type", "CameraComponent" },
		{ "enable", _c.enable },
		{ "fovY", _c.GetFovY() },
		{ "nearClip", _c.GetNearClip() },
		{ "farClip", _c.GetFarClip() },
		{ "cameraType", _c.GetCameraType() },
		{ "isMainCamera", _c.GetIsMainCamera() }
	};
}

void COMP_DEBUG::CameraDebug(CameraComponent* _camera) {
	if (!_camera) {
		return; // カメラがnullptrの場合は何もしない
	}

	float fovY = _camera->GetFovY();
	float nearClip = _camera->GetNearClip();
	float farClip = _camera->GetFarClip();

	/// param debug
	if (ImGui::DragFloat("fovY", &fovY, 0.01f, 0.1f, 3.14f)) {
		_camera->SetFovY(fovY);
	}

	if (ImGui::DragFloat("near clip", &nearClip, 0.01f, 0.01f, 100.0f)) {
		_camera->SetNearClip(nearClip);
	}

	if (ImGui::DragFloat("far clip", &farClip, 0.01f, 100.0f, 10000.0f)) {
		_camera->SetFarClip(farClip);
	}

	ImGui::Spacing();

	if (ImGui::Button("main camera setting")) {
		_camera->SetIsMainCamera(true);
	}

}
