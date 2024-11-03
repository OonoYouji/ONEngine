#pragma once

/// std
#include <string>
#include <variant>
#include <unordered_map>

/// lib
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include "Debugger/Assertion.h"


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
	/// 値をセットする
	/// </summary>
	/// <typeparam name="T">     : セットする変数の型      </typeparam>
	/// <param name="groupName"> : セットするグループの名前 </param>
	/// <param name="key">       : セットするキーの名前    </param>
	/// <param name="value">     : セットする値           </param>
	template <typename T>
	void SetValue(const std::string& groupName, const std::string& key, const T& value) {
		Group& group = groupArray_[groupName];

		Item newItem = value;
		group[key]   = newItem;
	}


	/// <summary>
	/// 値を追加する
	/// </summary>
	/// <typeparam name="T">     : 追加する値の型        </typeparam>
	/// <param name="groupName"> : 追加するグループの名前 </param>
	/// <param name="key">       : 追加するキーの名前    </param>
	/// <param name="value">     : 追加する値           </param>
	template <typename T>
	void AddValue(const std::string& groupName, const std::string& key, const T& value) {
		/// グループが未登録であれば登録する
		if(groupArray_.find(groupName) == groupArray_.end()) {
			//CreateGroup(groupName);
		}

		const Group& group = groupArray_.at(groupName);

		/// データが未登録なら登録する
		if(group.find(key) == group.end()) {
			SetValue(groupName, key, value);
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
	const T& GetValue(const std::string& groupName, const std::string& key) const {
		/// 指定groupがあるか確認
		Assert(groupArray_.find(groupName) != groupArray_.end(), "The specified group could not be found.");
		const Group& group = groupArray_.at(groupName);

		/// 指定のkeyがあるか確認
		Assert(group.find(key) != group.end(), "The specified key could not be found.");
		const Item& item = group.at(key);

		return std::get<T>(item);
	}


	void SaveJson(const std::string& filePath, const std::string& groupName);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	using Item  = std::variant<int, float, bool, Vec2, Vec3, Vec4>;
	using Group = std::unordered_map<std::string, Item>;

	std::unordered_map<std::string, Group> groupArray_;

private:
	VariableManager(const VariableManager&) = delete;
	VariableManager(VariableManager&&) = delete;
	VariableManager& operator= (const VariableManager&) = delete;
	VariableManager& operator= (VariableManager&&) = delete;
};