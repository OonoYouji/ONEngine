#include "VariableManager.h"

/// std
#include <cassert>
#include <fstream>
#include <sstream>

/// externals
#include <imgui.h>



void VariableManager::Initialize() {

}

void VariableManager::Finalize() {

}

void VariableManager::CreateGroup(const std::string& _groupName) {
	groupArray_[_groupName];
}


#pragma region DebuggingVarialbe
template<typename T>
void VariableManager::DebuggingVariable(const std::string& _label, T& _value) {}

template<>
void VariableManager::DebuggingVariable<float>(const std::string& _label, float& _value) {
	ImGui::DragFloat(_label.c_str(), &_value, 0.1f);
}

template<>
void VariableManager::DebuggingVariable<int>(const std::string& _label, int& _value) {
	ImGui::DragInt(_label.c_str(), &_value);
}

template<>
void VariableManager::DebuggingVariable<bool>(const std::string& _label, bool& _value) {
	ImGui::Checkbox(_label.c_str(), &_value);
}

template<>
void VariableManager::DebuggingVariable<Vec2>(const std::string& _label, Vec2& _value) {
	ImGui::DragFloat2(_label.c_str(), &_value.x, 0.1f);
}

template<>
void VariableManager::DebuggingVariable<Vec3>(const std::string& _label, Vec3& _value) {
	ImGui::DragFloat3(_label.c_str(), &_value.x, 0.1f);
}

template<>
void VariableManager::DebuggingVariable<Vec4>(const std::string& _label, Vec4& _value) {
	int flags = ImGuiColorEditFlags_AlphaBar;
	ImGui::ColorEdit4(_label.c_str(), &_value.x, flags);
}
#pragma endregion


#pragma region RegisterJson
template<typename T>
void VariableManager::RegisterToJson(json& _root, const std::string& key, const T& value) {
}

template<>
void VariableManager::RegisterToJson<float>(json& _root, const std::string& _key, const float& _value) {
	_root[_key] = _value;
}

template<>
void VariableManager::RegisterToJson<int>(json& _root, const std::string& _key, const int& _value) {
	_root[_key] = _value;
}

template<>
void VariableManager::RegisterToJson<bool>(json& _root, const std::string& _key, const bool& _value) {
	_root[_key] = _value;
}

template<>
void VariableManager::RegisterToJson<Vec2>(json& _root, const std::string& _key, const Vec2& _value) {
	_root[_key] = json::array({ _value.x, _value.y });;
}

template<>
void VariableManager::RegisterToJson<Vec3>(json& _root, const std::string& _key, const Vec3& _value) {
	_root[_key] = json::array({ _value.x, _value.y, _value.z });;
}

template<>
void VariableManager::RegisterToJson<Vec4>(json& _root, const std::string& _key, const Vec4& _value) {
	_root[_key] = json::array({ _value.x, _value.y, _value.z, _value.w });;
}
#pragma endregion



void VariableManager::DebuggingSpecificGroup(const std::string& _groupName) {
	auto itr = groupArray_.find(_groupName);

	//Assert(itr != groupArray_.end(), "The specified group could not be found.");
	if(itr == groupArray_.end()) {
		return;
	}

	Group& group = itr->second;

	for(auto& item : group) {
		std::visit([&](auto& value) {
			DebuggingVariable(item.first, value);
		}, item.second);
	}

}

void VariableManager::SaveSpecificGroupsToJson(const std::string& _filePath, const std::string& _groupName) {

	/// ---------------------------------------------------
	/// jsonに登録
	/// ---------------------------------------------------

	auto itr = groupArray_.find(_groupName);
	if(itr == groupArray_.end()) {
		return; /// 未登録は保存できないのでreturn
	}

	json root = json::object();

	root[_groupName] = json::object();
	json& groupRoot = root[_groupName];

	for(auto& item : itr->second) {
		std::visit([&](auto& value) {
			RegisterToJson(groupRoot, item.first, value);
		}, item.second);
	}


	/// ---------------------------------------------------
	/// ファイルに保存
	/// ---------------------------------------------------
	
	/// ディレクトリがなければ作成する
	std::filesystem::path dir(_filePath);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}


	std::string filePath = _filePath + "/" + _groupName + ".json";
	std::ofstream ofs;
	ofs.open(filePath);

	if(ofs.fail()) {
		Assert(false, "Failed open data file for write.");
		return;
	}

	///- ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void VariableManager::LoadSpecificGroupsToJson(const std::string& _filePath, const std::string& _groupName) {

	/// ---------------------------------------------------
	/// ファイルを開く
	/// ---------------------------------------------------

	std::string filePath = _filePath + "/" + _groupName + ".json";
	std::ifstream ifs;
	ifs.open(filePath);

	if(!ifs.is_open()) {
		Assert(false, "File could not be opened.");
		return;
	}

	/// json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();

	/// グループ検索
	json::iterator itrGroup = root.find(_groupName);

	/// 未登録チェック
	Assert(itrGroup != root.end(), "The group was unregistered.");

	///- 各アイテム
	for(json::iterator itrItem = itrGroup->begin(); itrItem != itrGroup->end(); ++itrItem) {

		///- アイテム名を取得
		const std::string& itemName = itrItem.key();

		/// int32_t型の値があれば
		if(itrItem->is_number_integer()) {
			int value = itrItem->get<int>();
			SetValue(_groupName, itemName, value);

		} else if(itrItem->is_number_float()) {
			double value = itrItem->get<double>();
			SetValue(_groupName, itemName, static_cast<float>(value));

		} else if(itrItem->is_boolean()) {
			bool value = itrItem->get<bool>();
			SetValue(_groupName, itemName, value);

		} else if(itrItem->is_array() && itrItem->size() == 2) {
			Vec2 value = { itrItem->at(0), itrItem->at(1) };
			SetValue(_groupName, itemName, value);

		} else if(itrItem->is_array() && itrItem->size() == 3) {
			Vec3 value = { itrItem->at(0), itrItem->at(1), itrItem->at(2) };
			SetValue(_groupName, itemName, value);

		} else if(itrItem->is_array() && itrItem->size() == 4) {
			Vec4 value = { itrItem->at(0), itrItem->at(1), itrItem->at(2), itrItem->at(3) };
			SetValue(_groupName, itemName, value);

		}

	}
}

void VariableManager::DebuggingGroupArray() {

}

