#include "Variables.h"

/// std
#include <fstream>
#include <filesystem>


/// engine
//#include "Engine/Core/Utility/Tools/VectorJsonConverter.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

using json = nlohmann::json;

namespace {

	///// ==================================================
	///// vector4 json
	///// ==================================================
	//void from_json(const json& j, Vec4& v) {
	//	v.x = j.at("x").get<float>();
	//	v.y = j.at("y").get<float>();
	//	v.z = j.at("z").get<float>();
	//	v.w = j.at("w").get<float>();
	//}

	//void to_json(json& j, const Vec4& v) {
	//	j = json{ { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
	//}

	///// ==================================================
	///// vector3 json
	///// ==================================================
	//void from_json(const json& j, Vec3& v) {
	//	v.x = j.at("x").get<float>();
	//	v.y = j.at("y").get<float>();
	//	v.z = j.at("z").get<float>();
	//}

	//void to_json(json& j, const Vec3& v) {
	//	j = json{ { "x", v.x }, { "y", v.y }, { "z", v.z } };
	//}

	///// ==================================================
	///// vector2 json
	///// ==================================================
	//void from_json(const json& j, Vec2& v) {
	//	v.x = j.at("x").get<float>();
	//	v.y = j.at("y").get<float>();
	//}

	//void to_json(json& j, const Vec2& v) {
	//	j = json{ { "x", v.x }, { "y", v.y } };
	//}

}



bool Variables::Has(const std::string& _name) const {
	return keyMap_.contains(_name);
}

void Variables::Remove(const std::string& _name) {
	if (!Has(_name)) {
		return;
	}

	size_t index = keyMap_.at(_name);
	variables_.erase(variables_.begin() + index);
	keyMap_.erase(_name);

	// Update indices in keyMap_ to avoid out of range errors
	for (auto& [key, value] : keyMap_) {
		if (value > index) {
			--value;
		}
	}
}

void Variables::Rename(const std::string& _oldName, const std::string& _newName) {
	if (!Has(_oldName)) {
		return;
	}

	auto index = keyMap_.at(_oldName);
	keyMap_.erase(_oldName);
	keyMap_[_newName] = index;
}

Variables::VarType Variables::GetType(const std::string& _name) {
	if (!Has(_name)) {
		return VarType::Unknown;
	}

	const auto& var = variables_[keyMap_.at(_name)];
	return std::visit([](auto&& arg) -> Variables::VarType {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, int>) {
			return VarType::kInt;
		} else if constexpr (std::is_same_v<T, float>) {
			return VarType::kFloat;
		} else if constexpr (std::is_same_v<T, bool>) {
			return VarType::kBool;
		} else if constexpr (std::is_same_v<T, std::string>) {
			return VarType::kString;
		} else if constexpr (std::is_same_v<T, Vec2>) {
			return VarType::kVector2;
		} else if constexpr (std::is_same_v<T, Vec3>) {
			return VarType::kVector3;
		} else if constexpr (std::is_same_v<T, Vec4>) {
			return VarType::kVector4;
		} else {
			return static_cast<int>(VarType::Unknown);
		}
		}, var
	);
}


void Variables::LoadJson(const std::string& _path) {
	/// .jsonファイルかチェック
	if (_path.find(".json") == std::string::npos) {
		return;
	}

	/// fileが存在するのかチェック
	if (!std::filesystem::exists(_path)) {
		return;
	}

	variables_.clear();
	nlohmann::json json;

	{	/// load json
		std::ifstream ifs(_path);
		ifs >> json;
		ifs.close();
	}

	for (auto& [key, value] : json.items()) {

		if (value.is_number_integer()) {
			Add(key, value.get<int>());

		} else if (value.is_number_float()) {
			Add(key, value.get<float>());

		} else if (value.is_boolean()) {
			Add(key, value.get<bool>());

		} else if (value.is_string()) {
			Add(key, value.get<std::string>());

		} else if (value.is_object() && value.contains("x") && value.contains("y") && value.contains("z") && value.contains("w")) {
			Vec4 vec = value.get<Vec4>();
			Add(key, vec);

		} else if (value.is_object() && value.contains("x") && value.contains("y") && value.contains("z")) {
			Vec3 vec = value.get<Vec3>();
			Add(key, vec);

		} else if (value.is_object() && value.contains("x") && value.contains("y")) {
			Vec2 vec = value.get<Vec2>();
			Add(key, vec);

		}

	}
}


void Variables::SaveJson(const std::string& _path) const {
	nlohmann::json json;
	for (const auto& [key, value] : keyMap_) {
		std::visit([&json, &key](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int>) {
				json[key] = arg;
			} else if constexpr (std::is_same_v<T, float>) {
				json[key] = arg;
			} else if constexpr (std::is_same_v<T, bool>) {
				json[key] = arg;
			} else if constexpr (std::is_same_v<T, std::string>) {
				json[key] = arg;
			} else if constexpr (std::is_same_v<T, Vec2>) {
				json[key] = arg;
			} else if constexpr (std::is_same_v<T, Vec3>) {
				json[key] = arg;
			} else if constexpr (std::is_same_v<T, Vec4>) {
				json[key] = arg;
			}
			}, variables_[value]);
	}

	std::filesystem::path path(_path);
	std::filesystem::create_directories(path.parent_path());

	std::ofstream ofs(_path);
	if (!ofs) {
		throw std::runtime_error("ファイルを開けませんでした: " + _path);
	}
	ofs << json.dump(4);
}