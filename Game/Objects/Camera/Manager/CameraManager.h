#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include <BaseCamera.h>


/// ===================================================
/// カメラの管理クラス
/// ===================================================
class CameraManager final {
	CameraManager() = default;
	~CameraManager() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static CameraManager* GetInstance();

	void AddCamera(const std::string& name, BaseCamera* camera);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, std::unique_ptr<BaseCamera>> cameras_;
	BaseCamera* mainCamera_ = nullptr;

private:
	CameraManager(const CameraManager&) = delete;
	CameraManager(CameraManager&&) = delete;
	CameraManager& operator=(const CameraManager&) = delete;
	CameraManager& operator=(CameraManager&&) = delete;
 };