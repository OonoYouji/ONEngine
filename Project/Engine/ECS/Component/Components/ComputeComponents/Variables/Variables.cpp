#include "Variables.h"

/// std
#include <fstream>
#include <filesystem>

/// external
#include <imgui.h>
#include <Externals/imgui/dialog/ImGuiFileDialog.h>

/// engine
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

using json = nlohmann::json;


void Variables::LoadJson(const std::string& _path) {
	/// .jsonファイルかチェック
	if (_path.find(".json") == std::string::npos) {
		return;
	}

	/// fileが存在するのかチェック
	if (!std::filesystem::exists(_path)) {
		return;
	}

	groupKeyMap_.clear();
	groups_.clear();
	nlohmann::json json;

	{	/// load json
		std::ifstream ifs(_path);
		ifs >> json;
		ifs.close();
	}

	for (auto& [groupKey, groupValue] : json.items()) {
		/// グループが存在しない場合は新規追加
		if (!HasGroup(groupKey)) {
			AddGroup(groupKey);
		}

		/// グループを取得
		Group& group = groups_[groupKeyMap_.at(groupKey)];

		for (auto& [varKey, varValue] : groupValue.items()) {
			/// 変数の型をチェックして追加
			if (varValue.is_number_integer()) {
				/// int
				group.Add(varKey, varValue.get<int>());
			} else if (varValue.is_number_float()) {
				/// float
				group.Add(varKey, varValue.get<float>());
			} else if (varValue.is_boolean()) {
				/// bool
				group.Add(varKey, varValue.get<bool>());
			} else if (varValue.is_string()) {
				/// string
				group.Add(varKey, varValue.get<std::string>());
			} else if (varValue.is_array() && varValue.size() == 2) {

				if (varValue[0].is_number_float() && varValue[1].is_number_float()) {
					group.Add(varKey, Vector2(varValue[0].get<float>(), varValue[1].get<float>()));
				} else if (varValue[0].is_number_float() && varValue[1].is_number_float()) {
					group.Add(varKey, Vector3(varValue[0].get<float>(), varValue[1].get<float>(), varValue[2].get<float>()));
				} else if (varValue[0].is_number_float() && varValue[1].is_number_float() && varValue[2].is_number_float() && varValue[3].is_number_float()) {
					group.Add(varKey, Vector4(varValue[0].get<float>(), varValue[1].get<float>(), varValue[2].get<float>(), varValue[3].get<float>()));
				}
			}
		}
	}

}


void Variables::SaveJson(const std::string& _path) {
	nlohmann::json json;

	/// スクリプトごとにgroupを生成する
	IEntity* owner = GetOwner();
	if (!owner) {
		Console::LogError("Variables::SaveJson();  owner is nullptr...");
		return;
	}


	for (const auto& [groupKey, value] : groupKeyMap_) {
		json[groupKey] = nlohmann::json::object();
		for (const auto& [varKey, varValue] : groups_[value].keyMap_) {

			std::visit([&json, &groupKey, &varKey](auto&& _arg) {
				using T = std::decay_t<decltype(_arg)>;
				if constexpr (std::is_same_v<T, int>) {
					json[groupKey][varKey] = _arg;
				} else if constexpr (std::is_same_v<T, float>) {
					json[groupKey][varKey] = _arg;
				} else if constexpr (std::is_same_v<T, bool>) {
					json[groupKey][varKey] = _arg;
				} else if constexpr (std::is_same_v<T, std::string>) {
					json[groupKey][varKey] = _arg;
				} else if constexpr (std::is_same_v<T, Vector2>) {
					json[groupKey][varKey] = _arg;
				} else if constexpr (std::is_same_v<T, Vector3>) {
					json[groupKey][varKey] = _arg;
				} else if constexpr (std::is_same_v<T, Vector4>) {
					json[groupKey][varKey] = _arg;
				}
				}, groups_[value].variables[varValue]);
		}
	}


	std::filesystem::path path(_path);
	std::filesystem::create_directories(path.parent_path());

	std::ofstream ofs(_path);
	if (!ofs) {
		throw std::runtime_error("ファイルを開けませんでした: " + _path);
	}
	ofs << json.dump(4);
}

void Variables::RegisterScriptVariables() {

	Script* script = GetOwner()->GetComponent<Script>();
	if (!script) {
		Console::LogError("Variables::SaveJson();  owner has no Script component...");
		return;
	}

	for (const auto& data : script->GetScriptDataList()) {
		size_t groupIndex = 0;

		/// 新規のグループを追加するか、既存のグループを取得する
		if (!HasGroup(data.scriptName)) {
			groupIndex = AddGroup(data.scriptName);
		} else {
			groupIndex = groupKeyMap_.at(data.scriptName);
		}

		/// スクリプトの変数をグループに追加
		Group& group = groups_[groupIndex];

		{
			MonoObject* safeObj = nullptr;
			if (data.gcHandle != 0) {
				safeObj = mono_gchandle_get_target(data.gcHandle);
			}

			if (!safeObj) {
				continue; //!< 対象のスクリプトがない場合はスキップ
			}

			MonoClass* monoClass = mono_object_get_class(safeObj);
			MonoClassField* field = nullptr;
			void* iter = nullptr;

			while ((field = mono_class_get_fields(monoClass, &iter))) {
				const char* fieldName = mono_field_get_name(field);

				// SerializeFieldチェックを削除
				MonoType* fieldType = mono_field_get_type(field);
				int type = mono_type_get_type(fieldType);

				switch (type) {
				case MONO_TYPE_I4: /// int
				{
					int value = 0;
					mono_field_get_value(safeObj, field, &value);
					group.Add(fieldName, value);
				}
				break;
				case MONO_TYPE_R4: /// float
				{
					float value = 0.0f;
					mono_field_get_value(safeObj, field, &value);
					group.Add(fieldName, value);
				}
				break;
				case MONO_TYPE_BOOLEAN: /// bool
				{
					bool value = false;
					mono_field_get_value(safeObj, field, &value);
					group.Add(fieldName, value);
				}
				break;
				case MONO_TYPE_STRING: /// string
				{
					MonoString* monoStr = nullptr;
					mono_field_get_value(safeObj, field, &monoStr);
					std::string value = mono_string_to_utf8(monoStr);
					group.Add(fieldName, value);
				}
				break;
				case MONO_TYPE_VALUETYPE: /// 構造体
				{
					MonoClass* fieldClass = mono_class_from_mono_type(fieldType);
					const char* className = mono_class_get_name(fieldClass);

					if (strcmp(className, "Vector2") == 0) {
						// Vector2
						Vector2 vec2;
						mono_field_get_value(safeObj, field, &vec2);
						group.Add(fieldName, vec2);

					} else if (strcmp(className, "Vector3") == 0) {
						// Vector3
						Vector3 vec3;
						mono_field_get_value(safeObj, field, &vec3);
						group.Add(fieldName, vec3);

					} else if (strcmp(className, "Vector4") == 0) {
						// Vector4
						Vector4 vec4;
						mono_field_get_value(safeObj, field, &vec4);
						group.Add(fieldName, vec4);

					}
				}
				break;
				}

			}
		}


	}
}

void Variables::SetScriptVariables(const std::string& _scriptName) {
	/* ----- スクリプトに対して変数の値を適用する ----- */

	IEntity* owner = GetOwner();
	if (!owner) {
		Console::LogError("Variables::SetScriptVariables();  owner is nullptr...");
		return;
	}

	Script* script = owner->GetComponent<Script>();
	if (!script) {
		Console::LogError("Variables::SetScriptVariables();  owner has no Script component...");
		return;
	}

	/// 一旦保存 (Cloneオブジェクト以外
	if (owner->GetId() > 0) {
		SaveJson("Assets/Jsons/" + owner->GetName() + ".json");
	}

	/// 適用の処理
	for (auto& data : script->GetScriptDataList()) {
		///!< 引数のスクリプト名と一致するかチェック
		if (data.scriptName != _scriptName) {
			continue;
		}

		/// 対象のスクリプトのデータを持っているかチェック
		if (!HasGroup(data.scriptName)) {
			continue;
		}

		Group& group = groups_[groupKeyMap_.at(data.scriptName)];


		/// C#側のオブジェクトを取得
		MonoObject* safeObj = nullptr;
		if (data.gcHandle != 0) {
			safeObj = mono_gchandle_get_target(data.gcHandle);
		}

		MonoClass* monoClass = mono_object_get_class(safeObj);
		MonoClassField* field = nullptr;
		void* iter = nullptr;

		while ((field = mono_class_get_fields(monoClass, &iter))) {
			const char* fieldName = mono_field_get_name(field);
			if (!group.Has(fieldName)) {
				continue;
			}

			auto& value = group.Get(fieldName);

			if (std::holds_alternative<int>(value)) {
				/// int
				int val = std::get<int>(value);
				mono_field_set_value(safeObj, field, &val);
			} else if (std::holds_alternative<float>(value)) {
				/// float
				float val = std::get<float>(value);
				mono_field_set_value(safeObj, field, &val);
			} else if (std::holds_alternative<bool>(value)) {
				/// bool
				bool val = std::get<bool>(value);
				mono_field_set_value(safeObj, field, &val);
			} else if (std::holds_alternative<std::string>(value)) {
				/// string
				const std::string& str = std::get<std::string>(value);
				MonoString* monoStr = mono_string_new(mono_domain_get(), str.c_str());
				mono_field_set_value(safeObj, field, monoStr);
			} else if (std::holds_alternative<Vector2>(value)) {
				/// Vector2
				Vector2 vec2 = std::get<Vector2>(value);
				mono_field_set_value(safeObj, field, &vec2);
			} else if (std::holds_alternative<Vector3>(value)) {
				/// Vector3
				Vector3 vec3 = std::get<Vector3>(value);
				mono_field_set_value(safeObj, field, &vec3);
			} else if (std::holds_alternative<Vector4>(value)) {
				/// Vector4
				Vector4 vec4 = std::get<Vector4>(value);
				mono_field_set_value(safeObj, field, &vec4);
			}
		}

	}

}

size_t Variables::AddGroup(const std::string& _name) {

	/// 同じ名前のグループがあるかチェック
	if (groupKeyMap_.contains(_name)) {
		return groupKeyMap_.at(_name);
	}

	/// 新規グループを作成
	Group group;
	group.name = _name;

	size_t index = groups_.size();
	groups_.push_back(group);
	groupKeyMap_[_name] = index;

	return index;
}

const Variables::Group& Variables::GetGroup(const std::string& _name) const {
	return groups_[groupKeyMap_.at(_name)];
}

bool Variables::HasGroup(const std::string& _name) const {
	return groupKeyMap_.contains(_name);
}

const std::unordered_map<std::string, size_t>& Variables::GetGroupKeyMap() const {
	return groupKeyMap_;
}

const std::vector<Variables::Group>& Variables::GetGroups() const {
	return groups_;
}




void COMP_DEBUG::VariablesDebug(Variables* _variables) {
	if (!_variables) {
		return;
	}

	static std::string variableName;

	//{	/// 新規変数の追加
	//	if (variableName.capacity() < 128) {
	//		variableName.reserve(128);
	//	}

	//	ImGui::SetNextItemWidth(128.0f);
	//	ImGuiInputText("##name", &variableName);

	//	ImGui::SameLine();
	//	ImGui::SetNextItemWidth(80.0f);
	//	static int type = 0;
	//	ImGui::Combo("##mold", &type, "int\0float\0bool\0string\0Vector2\0Vector3\0Vector4\0");
	//	ImGui::Spacing();


	//	ImGui::SameLine();
	//}

	ImGui::SeparatorText("");

	{	/// 既存の変数の表示

		std::list<std::pair<std::string, std::string>> removeList;
		std::vector<std::tuple<std::string, Variables::Var, std::string>> variables;
		std::string ptrStr, label;

		//for (const auto& [key, index] : _variables->GetKeyMap()) {
		//	variables.emplace_back(key, _variables->GetVariables()[index], "##{:p}" + std::to_string(reinterpret_cast<uintptr_t>(&_variables->GetVariables()[index])));
		//}

		//if (variables.empty()) {
		//	ImGui::Text("no variables...");
		//}


		for (auto& [name, variable, str] : variables) {
			ptrStr = str;
			label = name;

			ImGui::SetNextItemWidth(64.0f);
			if (ImGui::InputText((ptrStr + "string").c_str(), label.data(), label.capacity())) {
				label.resize(strlen(label.c_str()));
				removeList.push_back({ name, label });
			}

			ImGui::SameLine();

			/// 変数の型によって処理を変える
			//ValueImGui(_variables, ptrStr, name, variable.index());
		}

		//for (auto& [oldName, newName] : removeList) {
		//	_variables->Rename(oldName, newName);
		//}
	}

	ImGui::SeparatorText("");

	if (ImGui::Button("export")) {
		std::string ownerName = _variables->GetOwner()->GetName();

		_variables->RegisterScriptVariables();
		_variables->SaveJson("Assets/Jsons/" + ownerName + ".json");
	}

	ImGui::SameLine();

	// open Dialog Simple
	if (ImGui::Button("import")) {
		IGFD::FileDialogConfig config;
		config.path = "./Assets/Jsons";
		ImGuiFileDialog::Instance()->OpenDialog("Dialog", "Choose File", ".json", config);
	}
	// display
	if (ImGuiFileDialog::Instance()->Display("Dialog", ImGuiWindowFlags_NoDocking)) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			_variables->LoadJson(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

const Variables::Var& Variables::Group::Get(const std::string& _name) const {
	return variables[keyMap_.at(_name)];
}

bool Variables::Group::Has(const std::string& _name) const {
	return keyMap_.contains(_name);
}
