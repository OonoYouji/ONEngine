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

	SceneIO(class EntityComponentSystem* _ecs);
	~SceneIO();

	void Output(const std::string& _sceneName);
	void Input(const std::string& _sceneName);
	void OutputTemporary(const std::string& _sceneName);
	void InputTemporary(const std::string& _sceneName);

private:

	void SaveScene(const std::string& _filename);
	void LoadScene(const std::string& _filename);

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	class EntityComponentSystem* pECS_;
	IScene* scene_;

	std::string fileName_; // ioに使うファイル名
	std::string fileDirectory_;

};

