#include "CameraManager.h"



CameraManager* CameraManager::GetInstance() {
	static CameraManager instance;
	return &instance;
}


void CameraManager::AddCamera(const std::string& name, BaseCamera* camera) {
	std::unique_ptr<BaseCamera> add(camera);
	cameras_[name] = std::move(add);
}
