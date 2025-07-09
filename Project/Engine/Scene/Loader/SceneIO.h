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

	void Output(IScene* _scene);
	void Input(IScene* _scene);
	void OutputTemporary(IScene* _scene);
	void InputTemporary(IScene* _scene);

	void LoadEntity(const nlohmann::json& _entityJson, IEntity* _entity);

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

