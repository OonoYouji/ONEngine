#include "CameraComponent.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Math/Mathf.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"


namespace {
	struct Plane {
		Vector3 normal;
		float d;
	};

	struct Frustum {
		std::array<Plane, 6> planes;
	};

	Frustum CreateFrustumFromMatrix(const Matrix4x4& _matVP) {
		Frustum frustum;

		// Left
		frustum.planes[0].normal.x = _matVP.m[0][3] + _matVP.m[0][0];
		frustum.planes[0].normal.y = _matVP.m[1][3] + _matVP.m[1][0];
		frustum.planes[0].normal.z = _matVP.m[2][3] + _matVP.m[2][0];
		frustum.planes[0].d = _matVP.m[3][3] + _matVP.m[3][0];

		// Right
		frustum.planes[1].normal.x = _matVP.m[0][3] - _matVP.m[0][0];
		frustum.planes[1].normal.y = _matVP.m[1][3] - _matVP.m[1][0];
		frustum.planes[1].normal.z = _matVP.m[2][3] - _matVP.m[2][0];
		frustum.planes[1].d = _matVP.m[3][3] - _matVP.m[3][0];

		// Bottom
		frustum.planes[2].normal.x = _matVP.m[0][3] + _matVP.m[0][1];
		frustum.planes[2].normal.y = _matVP.m[1][3] + _matVP.m[1][1];
		frustum.planes[2].normal.z = _matVP.m[2][3] + _matVP.m[2][1];
		frustum.planes[2].d = _matVP.m[3][3] + _matVP.m[3][1];

		// Top
		frustum.planes[3].normal.x = _matVP.m[0][3] - _matVP.m[0][1];
		frustum.planes[3].normal.y = _matVP.m[1][3] - _matVP.m[1][1];
		frustum.planes[3].normal.z = _matVP.m[2][3] - _matVP.m[2][1];
		frustum.planes[3].d = _matVP.m[3][3] - _matVP.m[3][1];

		// Near
		frustum.planes[4].normal.x = _matVP.m[0][2];
		frustum.planes[4].normal.y = _matVP.m[1][2];
		frustum.planes[4].normal.z = _matVP.m[2][2];
		frustum.planes[4].d = _matVP.m[3][2];

		// Far
		frustum.planes[5].normal.x = _matVP.m[0][3] - _matVP.m[0][2];
		frustum.planes[5].normal.y = _matVP.m[1][3] - _matVP.m[1][2];
		frustum.planes[5].normal.z = _matVP.m[2][3] - _matVP.m[2][2];
		frustum.planes[5].d = _matVP.m[3][3] - _matVP.m[3][2];

		// 法線を正規化
		for (auto& p : frustum.planes) {
			p.normal = p.normal.Normalize();
			p.d /= p.normal.Len(); // Normalize 内で0除算済みでも安全のため
		}

		return frustum;
	}


	void ImGuiShowFrustum(const Frustum& frustum) {
		if (ImGui::CollapsingHeader("Frustum")) {
			const char* names[6] = { "Left", "Right", "Bottom", "Top", "Near", "Far" };
			std::string clipboardText;

			for (int i = 0; i < 6; ++i) {
				ImGui::Text("%s plane:", names[i]);
				ImGui::Text("  Normal: %.3f, %.3f, %.3f",
					frustum.planes[i].normal.x,
					frustum.planes[i].normal.y,
					frustum.planes[i].normal.z);
				ImGui::Text("  Distance: %.3f", frustum.planes[i].d);

				// クリップボード用文字列を作成
				clipboardText += names[i];
				clipboardText += " plane: ";
				clipboardText += "Normal(";
				clipboardText += std::to_string(frustum.planes[i].normal.x) + ", ";
				clipboardText += std::to_string(frustum.planes[i].normal.y) + ", ";
				clipboardText += std::to_string(frustum.planes[i].normal.z) + "), ";
				clipboardText += "Distance(" + std::to_string(frustum.planes[i].d) + ")\n";
			}

			// Clipboardコピー用ボタン
			if (ImGui::Button("Copy Frustum Data to Clipboard")) {
				ImGui::SetClipboardText(clipboardText.c_str());
			}
		}
	}


}	/// namespace


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

	/// type debug
	int cameraType = _camera->GetCameraType();
	if (ImGui::Combo("camera type", &cameraType, "3D\0 2D\0")) {
		_camera->SetCameraType(cameraType);
	}

	ImGui::Spacing();

	if (ImGui::Button("main camera setting")) {
		_camera->SetIsMainCameraRequest(true);
	}


	/// frustum debug
	ImGui::Checkbox("Draw Frustum", &_camera->isDrawFrustum_);

	if (ImGui::CollapsingHeader("Frustum Debug")) {
		Frustum frustum = CreateFrustumFromMatrix(_camera->GetViewProjection().matVP);
		ImGuiShowFrustum(frustum);
	}

	if (ImGui::CollapsingHeader("Matrix Debug")) {

		/// 行列のデバッグ表示
		const ViewProjection& vp = _camera->GetViewProjection();
		ImGui::Text("View Matrix:");
		for (int i = 0; i < 4; ++i) {
			ImGui::Text("%.3f %.3f %.3f %.3f", vp.matView.m[i][0], vp.matView.m[i][1], vp.matView.m[i][2], vp.matView.m[i][3]);
		}

		/// View行列をClipboardにコピーする
		if (ImGui::Button("Copy View Matrix to Clipboard")) {
			std::string viewMatrixStr;
			for (int i = 0; i < 4; ++i) {
				viewMatrixStr += std::format("{:.6f} {:.6f} {:.6f} {:.6f}\n", vp.matView.m[i][0], vp.matView.m[i][1], vp.matView.m[i][2], vp.matView.m[i][3]);
			}
			ImGui::SetClipboardText(viewMatrixStr.c_str());
		}


		ImGui::Text("Projection Matrix:");
		for (int i = 0; i < 4; ++i) {
			ImGui::Text("%.3f %.3f %.3f %.3f", vp.matProjection.m[i][0], vp.matProjection.m[i][1], vp.matProjection.m[i][2], vp.matProjection.m[i][3]);
		}

		/// Projection行列をClipboardにコピーする
		if (ImGui::Button("Copy Projection Matrix to Clipboard")) {
			std::string projectionMatrixStr;
			for (int i = 0; i < 4; ++i) {
				projectionMatrixStr += std::format("{:.6f} {:.6f} {:.6f} {:.6f}\n", vp.matProjection.m[i][0], vp.matProjection.m[i][1], vp.matProjection.m[i][2], vp.matProjection.m[i][3]);
			}
			ImGui::SetClipboardText(projectionMatrixStr.c_str());
		}
	}

}

void from_json(const nlohmann::json& _j, CameraComponent& _c) {
	_c.SetIsMainCameraRequest(_j.value("isMainCamera", true));
	_c.SetFovY(_j.value("fovY", 0.7f));
	_c.SetNearClip(_j.value("nearClip", 0.1f));
	_c.SetFarClip(_j.value("farClip", 1000.0f));
	_c.SetCameraType(
		_j.value("cameraType", static_cast<int>(CameraType::Type3D))
	);
	_c.isDrawFrustum_ = _j.value("isDrawFrustum", false);
}

void to_json(nlohmann::json& _j, const CameraComponent& _c) {
	_j = nlohmann::json{
		{ "type", "CameraComponent" },
		{ "enable", _c.enable },
		{ "fovY", _c.GetFovY() },
		{ "nearClip", _c.GetNearClip() },
		{ "farClip", _c.GetFarClip() },
		{ "cameraType", _c.GetCameraType() },
		{ "isMainCamera", _c.GetIsMainCameraRequest() },
		{ "isDrawFrustum", _c.isDrawFrustum_ }
	};
}



/// ///////////////////////////////////////////////////
/// カメラのコンポーネント
/// ///////////////////////////////////////////////////
CameraComponent::CameraComponent() {
	/// デフォルト値を設定
	SetFovY(0.7f);
	SetNearClip(0.1f);
	SetFarClip(1000.0f);
	SetIsMainCameraRequest(true);
	SetCameraType(static_cast<int>(CameraType::Type3D));
	isDrawFrustum_ = false;
}
CameraComponent::~CameraComponent() {}

void CameraComponent::UpdateViewProjection() {
	GameEntity* entity = GetOwner();
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

		matProjection_ = CameraMath::MakeOrthographicMatrix(
			-(EngineConfig::kWindowSize.x / 2.0f), (EngineConfig::kWindowSize.x / 2.0f),
			-(EngineConfig::kWindowSize.y / 2.0f), (EngineConfig::kWindowSize.y / 2.0f),
			nearClip_, farClip_
		);

	}

	viewProjection_.SetMappedData(ViewProjection(matView_ * matProjection_, matView_, matProjection_));

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

void CameraComponent::SetIsMainCameraRequest(bool _isMainCamera) {
	isMainCameraRequest_ = _isMainCamera;
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

bool CameraComponent::GetIsMainCameraRequest() const {
	return isMainCameraRequest_;
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

