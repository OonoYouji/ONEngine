#pragma once

/// std
#include <string>

/// engine
#include "../IScene.h"


/// ///////////////////////////////////////////////////
/// SceneのIOを行うクラス
/// ///////////////////////////////////////////////////
class SceneIO {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneIO(EntityComponentSystem* _ecs);
	~SceneIO();

	/// 入出力
	void Output(const std::string& _sceneName, class ECSGroup* _ecsGroup);
	void Input(const std::string& _sceneName);
	void OutputTemporary(const std::string& _sceneName, class ECSGroup* _ecsGroup);
	void InputTemporary(const std::string& _sceneName);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void SaveScene(const std::string& _filename, class ECSGroup* _ecsGroup);
	void LoadScene(const std::string& _filename);

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	EntityComponentSystem* pECS_; 
	IScene* scene_;

	std::string fileName_; // ioに使うファイル名
	std::string fileDirectory_;

};

