#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "Objects/Camera/Manager/BaseCamera.h"


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
	/// 終了処理
	/// </summary>
	void Finalize();

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
	void SetMainCamera(BaseCamera* camera);

	BaseCamera* GetCamera(const std::string& name);

	/// <summary>
	/// メインカメラのゲッタ
	/// </summary>
	/// <returns></returns>
	BaseCamera* GetMainCamera() const { 
		return mainCamera_; 
	}

	void SetSceneCamera(BaseCamera* _camera);
private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, BaseCamera*> cameras_;
	BaseCamera* mainCamera_ = nullptr;
	BaseCamera* beforeCamera_ = nullptr;
	BaseCamera* sceneCamera_ = nullptr;

private:
	CameraManager(const CameraManager&) = delete;
	CameraManager(CameraManager&&) = delete;
	CameraManager& operator=(const CameraManager&) = delete;
	CameraManager& operator=(CameraManager&&) = delete;
 };