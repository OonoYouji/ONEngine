#include "VariableManager.h"

/// externals
#include <nlohmann/json.hpp>
#include <imgui.h>

using namespace nlohmann;


void VariableManager::Initialize() {

}

void VariableManager::Finalize() {

}

void VariableManager::CreateGroup(const std::string& _groupName) {
	groupArray_[_groupName];
}


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


template<typename T>
void VariableManager::DebuggingVariable(const std::string& _label, T& _value) {
}



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

void VariableManager::SaveSpecificGroupsToJson(const std::string& filePath, const std::string& groupName) {

}

