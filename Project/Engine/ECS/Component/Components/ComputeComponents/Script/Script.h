#pragma once

/// std
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

/// externals
#include <nlohmann/json.hpp>
#include <jit/jit.h>

/// engine
#include "Engine/ECS/Component/Components/Interface/IComponent.h"


/// ///////////////////////////////////////////////////
/// スクリプトコンポーネント
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Script
 * @brief C#（Mono）スクリプトアセットを複数ロードし、ライフサイクル（Start, Update, CollisionEvent等）をC++側と同期実行・管理するスクリプトコンポーネントクラス
 */
class Script : public IComponent {
	friend class MonoScriptEngine;
	friend class ScriptUpdateSystem;
public:

    /**
     * @struct ScriptData
     * @brief 個々の登録されたC#スクリプトの実行メタ情報（ロードフラグ、有効フラグ、衝突コールバックメソッド参照）を保持する構造体
     */
	struct ScriptData {
		std::string scriptName;
		bool isAdded = false; ///< スクリプトが追加されたかどうか
		std::array<MonoMethod*, 3> collisionEventMethods = {};

		bool enable;

        /**
         * @brief スクリプトがC#インスタンスとして有効化されているかを取得します。
         */
		bool GetEnable(GameEntity* _entity);

        /**
         * @brief スクリプトのC#インスタンスとしての有効/無効を設定します。
         */
		void SetEnable(GameEntity* _entity, bool _enable);
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Script();

	/**
	 * @brief デストラクタ
	 */
	~Script() override;


	/**
	 * @brief 指定された名前のC#スクリプトが登録済みかを判定します。
	 */
	bool Contains(const std::string& _scriptName) const;

	/**
	 * @brief スクリプト名（クラス名）を指定して、C#スクリプトを新規に登録・アタッチします。
	 */
	void AddScript(const std::string& _scriptName);

	/**
	 * @brief 指定されたスクリプトをデタッチ・削除します。
	 */
	void RemoveScript(const std::string& _scriptName);


	/**
	 * @brief 登録インデックスからアタッチされているスクリプト名を取得します。
	 */
	const std::string& GetScriptName(size_t _index) const;

	/**
	 * @brief 現在アタッチされている全スクリプト名のリストを取得します。
	 */
	std::vector<std::string> GetScriptNames() const;


	/**
	 * @brief アタッチされているC#スクリプトデータ（メタ情報）の読み取り専用リストを取得します。
	 */
	const std::vector<ScriptData>& GetScriptDataList() const;

	/**
	 * @brief アタッチされているC#スクリプトデータ（メタ情報）のリストを取得します。
	 */
	std::vector<ScriptData>& GetScriptDataList();

	/**
	 * @brief 指定したスクリプト名に対応するメタデータへのポインタを取得します（無ければ nullptr）。
	 */
	ScriptData* GetScriptData(const std::string& _scriptName);


	/**
	 * @brief 指定したスクリプト名に対応するC#インスタンスの有効/無効を設定します。
	 */
	void SetEnable(const std::string& _scriptName, bool _enable);

	/**
	 * @brief 指定したスクリプト名に対応するC#インスタンスが有効であるかを取得します。
	 */
	bool GetEnable(const std::string& _scriptName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map< std::string, size_t> scriptIndexMap_;
	std::vector<ScriptData> scriptDataList_;
	bool isAdded_;


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/**
	 * @brief スクリプト側へエンティティのアタッチ登録が完了したかのフラグを設定します。
	 */
	void SetIsAdded(bool _added);

	/**
	 * @brief スクリプト側へエンティティのアタッチ登録が完了したかのフラグを取得します。
	 */
	bool GetIsAdded() const;

};

namespace ComponentDebug {
    /**
     * @brief エディタ用：Scriptコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
	void ScriptDebug(Script* _script);
}



/// ///////////////////////////////////////////////////
/// json用の関数
/// ///////////////////////////////////////////////////

/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, Script& _s);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const Script& _s);

} /// ONEngine
