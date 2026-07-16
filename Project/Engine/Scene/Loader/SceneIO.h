#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// ///////////////////////////////////////////////////
/// SceneのIOを行うクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class SceneIO
 * @brief シーンデータのシリアライズおよびデシリアライズ（入出力）を管理するクラス
 */
class SceneIO {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 * @param _ecs エンティティコンポーネントシステムへのポインタ
	 */
	SceneIO(class EntityComponentSystem* _ecs);

	/**
	 * @brief デストラクタ
	 */
	~SceneIO();

	/**
	 * @brief 指定したECSグループのエンティティをシーンファイルとして出力する
	 * @param _sceneName 出力するシーン名
	 * @param _ecsGroup 出力対象のECSグループ
	 */
	void Output(const std::string& _sceneName, class ECSGroup* _ecsGroup);

	/**
	 * @brief 指定したシーンファイルを読み込み、ECSグループに復元する
	 * @param _sceneName 読み込むシーン名
	 * @param _ecsGroup 復元対象のECSグループ
	 */
	void Input(const std::string& _sceneName, class ECSGroup* _ecsGroup);

	/**
	 * @brief ECSグループのデータをメモリ上に一時保存（シリアライズ）する
	 * @param _ecsGroup 保存対象のECSグループ
	 */
	void OutputTemporary(class ECSGroup* _ecsGroup);

	/**
	 * @brief メモリ上に一時保存されたデータからECSグループを復元する
	 * @param _ecsGroup 復元対象のECSグループ
	 */
	void InputTemporary(class ECSGroup* _ecsGroup);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief シーンファイルを物理ファイル群（シーン情報と各エンティティファイル）として保存する
	 * @param _filename 保存先ファイル名
	 * @param _ecsGroup 保存対象のECSグループ
	 */
	void SaveScene(const std::string& _filename, class ECSGroup* _ecsGroup);

	/**
	 * @brief シーンファイルを読み込み、各エンティティファイルをパースしてロードする
	 * @param _filename 読み込み元ファイル名
	 * @param _ecsGroup ロード先のECSグループ
	 */
	void LoadScene(const std::string& _filename, class ECSGroup* _ecsGroup);

	/**
	 * @brief ECSグループ内の有効なエンティティを走査し、JSONオブジェクトにシリアライズする
	 * @param _output 出力先JSONオブジェクト
	 * @param _ecsGroup 対象のECSグループ
	 */
	void SaveSceneToJson(nlohmann::json& _output, class ECSGroup* _ecsGroup);

	/**
	 * @brief JSONオブジェクトからエンティティやコンポーネント、親子関係をパースして復元する
	 * @param _input 入力元JSONオブジェクト
	 * @param _ecsGroup ロード先のECSグループ
	 */
	void LoadSceneFromJson(const nlohmann::json& _input, class ECSGroup* _ecsGroup);

	/**
	 * @brief 指定されたJSONオブジェクトを指定ファイルに整形出力する
	 * @param _json 保存対象のJSONオブジェクト
	 * @param _filename 保存先ファイル名
	 */
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


} /// ONEngine
