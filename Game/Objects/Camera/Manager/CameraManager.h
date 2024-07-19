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

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static CameraManager* GetInstance();

	/// <summary>
	/// mapのカメラを更新
	/// </summary>
	void Update();

	/// <summary>
	/// BaseCameraをmapに追加
	/// </summary>
	/// <param name="name"> : map_key</param>
	/// <param name="camera"> : camera pointer</param>
	void AddCamera(const std::string& name, BaseCamera* camera);

	/// <summary>
	/// メインカメラのセット
	/// </summary>
	/// <param name="name"></param>
	void SetMainCamera(const std::string& name);

	/// <summary>
	/// メインカメラのゲッタ
	/// </summary>
	/// <returns></returns>
	BaseCamera* GetMainCamera() const { return mainCamera_; }

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