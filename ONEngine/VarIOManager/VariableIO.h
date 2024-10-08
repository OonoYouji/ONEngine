#pragma once

#include <json.hpp>

#include <variant>
#include <string>
#include <unordered_map>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"


using json = nlohmann::json;

/// <summary>
/// 項目
/// </summary>
using Item = std::variant<bool, int, float, Vec2, Vec3, Vec4>;

/// <summary>
/// グループ
/// </summary>
using Group = std::unordered_map<std::string, Item>;


class VariableIO final {
	VariableIO() {}
	~VariableIO() {}
public:

	static VariableIO* GetInstance() {
		static VariableIO instance;
		return &instance;
	}

	void Update();

	void ImGuiDebug();

	void CreateGroup(const std::string& groupName);

	/// <summary>
	/// 値のセット
	/// </summary>
	template<typename T>
	void SetValue(const std::string& groupName, const std::string& key, const T& value);

	/// <summary>
	/// 値の追加
	/// </summary>
	template<typename T>
	void AddItem(const std::string& groupName, const std::string& key, const T& value);


	/// <summary>
	/// int32_t型の値を取得
	/// </summary>
	template<typename T>
	const T& GetValue(const std::string& groupName, const std::string& key) const;


	/// <summary>
	/// ファイルに書き出し
	/// </summary>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// グループの読み込み
	/// </summary>
	/// <param name="groupName"></param>
	void LoadFile(const std::string& groupName);

private:

	/// 追い出す先のファイル
	const std::string kDirectoryPath_ = "./Resources/Parameters/";

	std::unordered_map<std::string, Group> datas_;

private:
	VariableIO(const VariableIO&)             = delete;
	VariableIO(VariableIO&&)                  = delete;
	VariableIO& operator= (const VariableIO&) = delete;
	VariableIO& operator= (VariableIO&&)      = delete;
};
