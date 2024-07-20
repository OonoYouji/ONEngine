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

	///- listにあるカメラを更新
	for(auto& camera : cameras_) {
		if(camera.second->GetIsAcitve()) {
			camera.second->Update();
		}
	}

}


/// ===================================================
/// カメラの追加
/// ===================================================
void CameraManager::AddCamera(const std::string& name, BaseCamera* camera) {
	std::unique_ptr<BaseCamera> add(camera);
	cameras_[name] = std::move(add);
}


/// ===================================================
/// メインカメラのセット
/// ===================================================
void CameraManager::SetMainCamera(const std::string& name) {
	mainCamera_ = cameras_.at(name).get();
}


