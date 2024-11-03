#pragma once

/// std
#include <string>
#include <variant>
#include <unordered_map>

/// lib
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"



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

	void ImGui();


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