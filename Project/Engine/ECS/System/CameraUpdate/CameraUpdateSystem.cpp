#include "CameraUpdateSystem.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


namespace {

	void DrawFrustumDebug(const CameraComponent* _camera, float _size = 1.0f) {
		/// ----- 試錐台のデバッグ表示 ----- ///



		const size_t kNDCCornerCount = 8;

		/// NDC空間の8頂点
		std::array<Vector4, kNDCCornerCount> ndcCorners = {
			Vector4(-1, -1, 0, 1), // Near Bottom Left
			Vector4(1, -1, 0, 1), // Near Bottom Right
			Vector4(1, 1, 0, 1), // Near Top Right
			Vector4(-1, 1, 0, 1), // Near Top Left
			Vector4(-1, -1, 1, 1), // Far Bottom Left
			Vector4(1, -1, 1, 1), // Far Bottom Right
			Vector4(1, 1, 1, 1), // Far Top Right
			Vector4(-1, 1, 1, 1)  // Far Top Left
		};


		/// ViewProjectionの逆行列を計算
		Matrix4x4 invVP = _camera->GetViewProjection().matVP.Inverse();
		const Vector3 camPos = _camera->GetOwner()->GetPosition();

		std::array<Vector4, kNDCCornerCount> worldCorners;
		for (size_t i = 0; i < kNDCCornerCount; i++) {
			Vector4 wc = ndcCorners[i] * invVP;
			if (wc.w != 0.0f) {
				wc.x /= wc.w;
				wc.y /= wc.w;
				wc.z /= wc.w;
			}

			Vector3 dir = {
				wc.x - camPos.x,
				wc.y - camPos.y,
				wc.z - camPos.z
			};

			wc.x = camPos.x + dir.x * _size;
			wc.y = camPos.y + dir.y * _size;
			wc.z = camPos.z + dir.z * _size;

			worldCorners[i] = wc;
		}

		/// 12本の線分の頂点ペア
		const std::array<std::pair<int, int>, 12> edges = { {
			{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, // near
			{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, // far
			{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }  // side
			} };

		using Line = std::pair<Vector3, Vector3>;
		std::vector<Line> lines;
		lines.reserve(12);
		for (const auto& edge : edges) {
			Vector3 start = Vector3(worldCorners[edge.first].x, worldCorners[edge.first].y, worldCorners[edge.first].z);
			Vector3 end = Vector3(worldCorners[edge.second].x, worldCorners[edge.second].y, worldCorners[edge.second].z);
			lines.emplace_back(start, end);
		}


		for (const auto& line : lines) {
			Gizmo::DrawLine(line.first, line.second, Vector4(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色
		}

	}

}	/// namespace


CameraUpdateSystem::CameraUpdateSystem(DxDevice* _dxDevice) : pDxDevice_(_dxDevice) {
	pMainCamera_ = nullptr;
	pMainCamera2D_ = nullptr;
}

void CameraUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {

	/// カメラのComponentを集める
	ComponentArray<CameraComponent>* cameraArray = _ecs->GetComponentArray<CameraComponent>();
	if (!cameraArray || cameraArray->GetUsedComponents().empty()) {
		return; /// カメラのコンポーネント配列が存在しない場合は何もしない
	}

	for (auto& cameraComponent : cameraArray->GetUsedComponents()) {
		if (!cameraComponent) {
			continue; /// nullptrチェック
		}

		/// ViewProjectionが作られていなければ作成する
		if (!cameraComponent->IsMakeViewProjection()) {
			cameraComponent->MakeViewProjection(pDxDevice_);
		}

		/// カメラのViewProjectionを更新
		cameraComponent->UpdateViewProjection();

		/// フラスタムのデバッグ表示
		if (cameraComponent->isDrawFrustum_) {
			DrawFrustumDebug(cameraComponent, 0.1f);
		}


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
					if (pMainCamera2D_ && pMainCamera2D_->cameraType_ == static_cast<int>(CameraType::Type2D)) {
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
