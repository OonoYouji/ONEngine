#include "CameraManager.h"


/// ===================================================
/// インスタンス確保
/// ===================================================
CameraManager* CameraManager::GetInstance() {
	static CameraManager instance;
	return &instance;
}


/// ===================================================
/// 終了処理
/// ===================================================
void CameraManager::Finalize() {
	cameras_.clear();
}


/// ===================================================
/// 更新処理
/// ===================================================
void CameraManager::Update() {

	/// 毎フレームsceneCameraをメインカメラにセット
	SetMainCamera(sceneCamera_);

#ifdef _DEBUG

	BaseCamera* debugCamera = cameras_.at("DebugCamera");
	if(debugCamera->isActive) {
		if(GetMainCamera() != debugCamera) {
			beforeCamera_ = GetMainCamera();
		}
		SetMainCamera(debugCamera);
	} else {
		if(beforeCamera_) {
			SetMainCamera(beforeCamera_);
		}
	}
#endif // _DEBUG

}


/// ===================================================
/// カメラの追加
/// ===================================================
void CameraManager::AddCamera(const std::string& name, BaseCamera* camera) {
	cameras_[name] = camera;
}


/// ===================================================
/// メインカメラのセット
/// ===================================================
void CameraManager::SetMainCamera(const std::string& name) {
	mainCamera_ = cameras_.at(name);
}

void CameraManager::SetMainCamera(BaseCamera* camera) {
	mainCamera_ = camera;
}

BaseCamera* CameraManager::GetCamera(const std::string& name) {
	return cameras_.at(name);
}

void CameraManager::SetSceneCamera(BaseCamera* _camera) {
	sceneCamera_ = _camera;
}


