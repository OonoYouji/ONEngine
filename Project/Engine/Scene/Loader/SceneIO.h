#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

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

	/// 入出力
	void Output(const std::string& _sceneName, class ECSGroup* _ecsGroup);
	void Input(const std::string& _sceneName, class ECSGroup* _ecsGroup);

	/// 仮のシーンの入出力
	void OutputTemporary(const std::string& _sceneName, class ECSGroup* _ecsGroup);
	void InputTemporary(const std::string& _sceneName, class ECSGroup* _ecsGroup);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// シーンの入出力
	void SaveScene(const std::string& _filename, class ECSGroup* _ecsGroup);
	void LoadScene(const std::string& _filename, class ECSGroup* _ecsGroup);

	/// ECSGroupをJsonに変換する
	void SaveSceneToJson(nlohmann::json& _output, class ECSGroup* _ecsGroup);
	void LoadSceneFromJson(const nlohmann::json& _input, class ECSGroup* _ecsGroup);

	/// Jsonの入出力
	void OutputJson(const nlohmann::json& _json, const std::string& _filename);

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	/// ----- other class ----- ///
	class EntityComponentSystem* pEcs_; 

	std::string fileName_; // ioに使うファイル名
	std::string fileDirectory_;

	/// 一時的なシーンの保存に使う、ファイルとして保存はしない
	nlohmann::json tempSceneJson_;

};

