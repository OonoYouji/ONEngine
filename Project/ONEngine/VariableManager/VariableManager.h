#pragma once

/// std
#include <string>
#include <variant>
#include <unordered_map>

/// externals
#include <nlohmann/json.hpp>

/// lib
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include "Debugger/Assertion.h"


using namespace nlohmann;

/// ===================================================
/// 変数をjsonで管理するクラス
/// ===================================================
class VariableManager final {
	VariableManager() {}
	~VariableManager() {}
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static VariableManager* GetInstance() {
		static VariableManager instance;
		return &instance;
	}

	void Initialize();
	void Finalize();


	/// <summary>
	/// 新規グループの作成
	/// </summary>
	/// <param name="groupName"> : 作成するグループの名前 </param>
	void CreateGroup(const std::string& _groupName);

	/// <summary>
	/// 値をセットする
	/// </summary>
	/// <typeparam name="T">     : セットする変数の型      </typeparam>
	/// <param name="groupName"> : セットするグループの名前 </param>
	/// <param name="key">       : セットするキーの名前    </param>
	/// <param name="value">     : セットする値           </param>
	template <typename T>
	void SetValue(const std::string& _groupName, const std::string& _key, const T& _value) {
		Group& group = groupArray_[_groupName];

		Item newItem = _value;
		group[_key]   = newItem;
	}


	/// <summary>
	/// 値を追加する
	/// </summary>
	/// <typeparam name="T">     : 追加する値の型        </typeparam>
	/// <param name="groupName"> : 追加するグループの名前 </param>
	/// <param name="key">       : 追加するキーの名前    </param>
	/// <param name="value">     : 追加する値           </param>
	template <typename T>
	void AddValue(const std::string& _groupName, const std::string& _key, const T& _value) {
		/// グループが未登録であれば登録する
		if(groupArray_.find(_groupName) == groupArray_.end()) {
			CreateGroup(_groupName);
		}

		const Group& group = groupArray_.at(_groupName);

		/// データが未登録なら登録する
		if(group.find(_key) == group.end()) {
			SetValue(_groupName, _key, _value);
		}
	}


	/// <summary>
	/// 値のゲット
	/// </summary>
	/// <typeparam name="T">     : ゲットする変数の型             </typeparam>
	/// <param name="groupName"> : ゲットするグループの名前        </param>
	/// <param name="key">       : ゲットするキーの名前           </param>
	/// <returns>         return : 引数で指定したグループのキーの値 </returns>
	template <typename T>
	const T& GetValue(const std::string& _groupName, const std::string& _key) const {
		/// 指定groupがあるか確認
		Assert(groupArray_.find(_groupName) != groupArray_.end(), "The specified group could not be found.");
		const Group& group = groupArray_.at(_groupName);

		/// 指定のkeyがあるか確認
		Assert(group.find(_key) != group.end(), "The specified key could not be found.");
		const Item& item = group.at(_key);

		return std::get<T>(item);
	}


	/// <summary>
	/// 変数のデバッグ
	/// </summary>
	/// <typeparam name="T">  : 変数の型         </typeparam>
	/// <param name="_label"> : imguiに使うラベル </param>
	/// <param name="_value"> : 編集する値        </param>
	template <typename T>
	void DebuggingVariable(const std::string& _label, T& _value);

	/// <summary>
	/// 特定のグループのimgui debugの表示
	/// </summary>
	/// <param name="groupName"></param>
	void DebuggingSpecificGroup(const std::string& _groupName);

	/// <summary>
	/// jsonに値を登録する関数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="_root"></param>
	/// <param name="key"></param>
	/// <param name="value"></param>
	template <typename T>
	void RegisterToJson(json& _root, const std::string& key, const T& value);

	/// <summary>
	/// データをjson形式で値を保存
	/// </summary>
	/// <param name="_filePath">  : 保存先のファイルパス   </param>
	/// <param name="_groupName"> : 保存するグループの名前 </param>
	void SaveSpecificGroupsToJson(const std::string& _filePath, const std::string& _groupName);

	/// <summary>
	/// 特定のグループのjsonを読み込む
	/// </summary>
	/// <param name="_filePath"></param>
	/// <param name="_groupName"></param>
	void LoadSpecificGroupsToJson(const std::string& _filePath, const std::string& _groupName);



	/// <summary>
	/// group arrayのデバッグ
	/// </summary>
	void DebuggingGroupArray();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	using Item  = std::variant<int, float, bool, Vec2, Vec3, Vec4, std::string>;
	using Group = std::unordered_map<std::string, Item>;

	std::unordered_map<std::string, Group> groupArray_;

private:
	VariableManager(const VariableManager&) = delete;
	VariableManager(VariableManager&&) = delete;
	VariableManager& operator= (const VariableManager&) = delete;
	VariableManager& operator= (VariableManager&&) = delete;
};

