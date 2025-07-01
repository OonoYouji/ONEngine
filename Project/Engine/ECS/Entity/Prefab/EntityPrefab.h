#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// ///////////////////////////////////////////////////
/// エンティティのPrefab
/// ///////////////////////////////////////////////////
class EntityPrefab final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief JSONファイルを読み込んでPrefabを生成する
	/// @param _path .jsonへのパス
	EntityPrefab(const std::string& _path);
	~EntityPrefab();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string path_;
	nlohmann::json json_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	const nlohmann::json& GetJson() const { return json_; }

};

