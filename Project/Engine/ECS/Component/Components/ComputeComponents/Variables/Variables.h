#pragma once

/// std
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>

/// external
#include <Externals/nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Tools/Log.h"

namespace ONEngine {
class Variables;

/// Json変換
void from_json(const nlohmann::json& _j, class Variables& _g);
void to_json(nlohmann::json& _j, const class Variables& _g);


/// ///////////////////////////////////////////////////
/// 変数component
/// ///////////////////////////////////////////////////
class Variables : public IComponent {
	friend void from_json(const nlohmann::json& _j, Variables& _v);
	friend void to_json(nlohmann::json& _j, const Variables& _v);
public:
	/// ================================================
	/// public : sub class
	/// ================================================

	enum class VarType {
		kInt,
		kFloat,
		kBool,
		kString,
		kVector2,
		kVector3,
		kVector4,
		kIntList,
		kFloatList,
		kBoolList,
		kStringList,
		kVector3List,
		kObject,
		kObjectList,
		Unknown
	};

	struct GenericObject;

	using Var = std::variant<
		int, float, bool, std::string, Vector2, Vector3, Vector4,
		std::vector<int>, std::vector<float>, std::vector<bool>, std::vector<std::string>, std::vector<Vector3>,
		std::shared_ptr<GenericObject>, std::vector<std::shared_ptr<GenericObject>>
	>;

	struct GenericObject {
		std::string typeName;
		std::map<std::string, Var> fields;

		/**
		 * @brief フィールドに変数を追加または更新します。
		 */
		void Add(const std::string& _name, const Var& _value) {
			fields[_name] = _value;
		}

		/**
		 * @brief 指定されたフィールドが存在するかを判定します。
		 */
		bool Has(const std::string& _name) const {
			return fields.contains(_name);
		}
	};

	/**
	 * @brief Mono上のC#オブジェクトをエンジン用のVar（std::variant）に変換します。
	 */
	static Var MonoObjectToVar(void* obj, void* type);

	/**
	 * @brief Mono上のC#オブジェクトを汎用データ構造（GenericObject）に変換します。
	 */
	static std::shared_ptr<GenericObject> MonoObjectToGeneric(void* obj);

	/**
	 * @brief エンジン用のVarからC#上のMonoオブジェクトへ値を設定・変換します。
	 */
	static void VarToMonoObject(void* obj, void* klass, const Var& var);


	/// @brief 変数のグループ、スクリプトごとに使用する予定
	struct Group {
		std::string name; ///< グループ名
		std::map<std::string, size_t> keyMap;
		std::vector<Var> variables; ///< グループに属する変数

		/**
		 * @brief グループに変数を追加・更新します。
		 */
		template <typename T = Var>
		void Add(const std::string& _name, const T& _value) {
			if (keyMap.contains(_name)) {
				variables[keyMap[_name]] = _value;
				return;
			}

			keyMap[_name] = variables.size();
			variables.emplace_back(_value);
		}

		/**
		 * @brief 指定された型の変数を取得します（非const）。
		 */
		template <typename T>
		T& Get(const std::string& _name) {
			return std::get<T>(variables[keyMap.at(_name)]);
		}

		/**
		 * @brief 指定された型の変数を取得します（const）。
		 */
		template <typename T>
		const T& Get(const std::string& _name) const {
			return std::get<T>(variables[keyMap.at(_name)]);
		}

		/**
		 * @brief 変数の生のvariant参照を取得します。
		 */
		const Var& Get(const std::string& _name) const;

		/**
		 * @brief 指定された変数がグループに登録されているかを判定します。
		 */
		bool Has(const std::string& _name) const;


	};


public:
	/// ================================================
	/// public : methods
	/// ================================================


	/**
	 * @brief コンストラクタ
	 */
	Variables();

	/**
	 * @brief デストラクタ
	 */
	~Variables() override;


	/**
	 * @brief JSONファイルを読み込んで管理変数を設定します。
	 * @param _path JSONファイルパス
	 */
	void LoadJson(const std::string& _path);

	/**
	 * @brief 管理変数をJSONファイルとして書き出して保存します。
	 * @param _path JSONファイルパス
	 */
	void SaveJson(const std::string& _path);


	/**
	 * @brief スクリプト上のシリアライズ対象のフィールド変数を自動登録します。
	 */
	void RegisterScriptVariables();

	/**
	 * @brief スクリプト側の型構造に合わせ、登録済みのスクリプト変数を再読み込み（同期）します。
	 */
	void ReloadScriptVariables();

	/**
	 * @brief スクリプト実行環境（C#インスタンスなど）に変数の値を設定・同期させます。
	 * @param _scriptName 対象のスクリプトクラス名
	 */
	void SetScriptVariables(const std::string& _scriptName);


	/**
	 * @brief 変数のグループ(スクリプト単位)を追加します。
	 * @param _name グループ名
	 * @return 追加されたGroupのインデックス
	 */
	size_t AddGroup(const std::string& _name);

	/**
	 * @brief 指定したグループ名に対応するグループ情報を取得します。
	 * @param _name 取得するグループの名前
	 * @return Group参照
	 */
	const Group& GetGroup(const std::string& _name) const;

	/**
	 * @brief 指定された名前のグループが存在するかを判定します。
	 */
	bool HasGroup(const std::string& _name) const;


	/**
	 * @brief グループ名とインデックスマップへの参照を取得します。
	 */
	const std::unordered_map<std::string, size_t>& GetGroupKeyMap() const;

	/**
	 * @brief 全グループの配列リストを取得します。
	 */
	const std::vector<Group>& GetGroups() const;

	/**
	 * @brief 指定されたグループ内の変数値を明示的に上書きまたは新規追加設定します。
	 * @param _groupName 対象のグループ名
	 * @param _varName 対象の変数名
	 * @param _value 設定する変数の値
	 */
	void SetVariable(const std::string& _groupName, const std::string& _varName, const Var& _value);

private:
	/// ================================================
	/// private : objects
	/// ================================================

	std::unordered_map<std::string, size_t> groupKeyMap_; /// グループ名とインデックスのマップ
	std::vector<Group> groups_; /// 変数のグループ
};


/// ==================================================
/// public : methods
/// ==================================================


/// @brief ComponentのDebug
namespace ComponentDebug {
	/**
	 * @brief エディタ用：Variablesコンポーネントが保持する変数のインスペクタデバッグ表示処理を行います。
	 */
	void VariablesDebug(Variables* _variables);
}

} /// ONEngine
