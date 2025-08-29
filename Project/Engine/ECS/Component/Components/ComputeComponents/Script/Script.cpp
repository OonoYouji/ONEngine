#include "Script.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiShowField.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

using namespace CSGui;



Script::Script() {
	SetIsAdded(false);
}

Script::~Script() {}

void Script::AddScript(const std::string& _scriptName) {
	/// すでにアタッチされているかチェック
	if (scriptIndexMap_.contains(_scriptName)) {
		return;
	}

	ScriptData newScriptData;
	newScriptData.scriptName = _scriptName;

	/// インデックスを登録
	scriptIndexMap_[_scriptName] = scriptDataList_.size();
	scriptDataList_.push_back(std::move(newScriptData));
}

bool Script::Contains(const std::string& _scriptName) const {
	if (scriptIndexMap_.contains(_scriptName)) {
		return true;
	}

	return false;
}

void Script::RemoveScript(const std::string& _scriptName) {
	if (!scriptIndexMap_.contains(_scriptName)) {
		Console::LogWarning("Script " + _scriptName + " not found, cannot remove.");
		return;
	}

	size_t index = scriptIndexMap_[_scriptName];
	/// vectorの要素を削除するのに合わせてmapのインデックスも更新
	scriptIndexMap_.erase(_scriptName);
	for (auto& [name, value] : scriptIndexMap_) {
		if (value > index) {
			--value;  ///< 削除した分インデックスをずらす
		}
	}

	scriptDataList_.erase(scriptDataList_.begin() + index);
}

const std::string& Script::GetScriptName(size_t _index) const {
	if (_index < scriptDataList_.size()) {
		return scriptDataList_[_index].scriptName;
	}

	Console::Log("Index out of range in GetScriptName");
	return scriptDataList_[0].scriptName;
}

std::vector<std::string> Script::GetScriptNames() const {
	std::vector<std::string> scriptNames;
	for (auto& script : scriptDataList_) {
		scriptNames.push_back(script.scriptName);
	}

	return scriptNames;
}

const std::vector<Script::ScriptData>& Script::GetScriptDataList() const {
	return scriptDataList_;
}

std::vector<Script::ScriptData>& Script::GetScriptDataList() {
	return scriptDataList_;
}

Script::ScriptData* Script::GetScriptData(const std::string& _scriptName) {
	for (auto& data : scriptDataList_) {
		if (data.scriptName == _scriptName) {
			return &data;  ///< 一致するスクリプトデータを返す
		}
	}

	Console::LogError("Script::GetScriptData - Script " + _scriptName + " not found in GetScriptData.");
	return nullptr;
}

void Script::SetEnable(const std::string& _scriptName, bool _enable) {
	/// スクリプト名が一致するものを探す
	for (auto& script : scriptDataList_) {
		if (script.scriptName == _scriptName) {
			script.enable = _enable;
			Console::Log("Script " + _scriptName + " enable set to " + std::to_string(_enable));
			return;
		}
	}


	/// 見つからなかった場合
	Console::LogWarning("Script " + _scriptName + " not found, cannot set enable state.");
}

bool Script::GetEnable(const std::string& _scriptName) {
	/// スクリプト名が一致するものを探す
	for (const auto& script : scriptDataList_) {
		if (script.scriptName == _scriptName) {
			return script.enable;
		}
	}

	/// 見つからなかった場合
	return false;
}

void Script::SetIsAdded(bool _added) {
	isAdded_ = _added;
}

bool Script::GetIsAdded() const {
	return isAdded_;
}

void COMP_DEBUG::ScriptDebug(Script* _script) {
	if (!_script) {
		return;
	}

	std::string ptrLable;
	std::vector<Script::ScriptData>& scriptList = _script->GetScriptDataList();
	for (size_t i = 0; i < scriptList.size(); i++) {
		auto& script = scriptList[i];

		ptrLable = "##" + std::to_string(reinterpret_cast<uintptr_t>(&script));

		/// 有効/無効のチェックボックス
		ImGui::Checkbox(ptrLable.c_str(), &script.enable);

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();


		/// 有効/無効に応じてテキストの色を変える
		if (!script.enable) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		}


		if (ImGui::CollapsingHeader(script.scriptName.c_str())) {
			/// ------------------------------------------------------------------
			/// スクリプト内の[SerializeField]など表示
			/// ------------------------------------------------------------------

			GameEntity* entity = _script->GetOwner();
			MonoScriptEngine* monoEngine = GetMonoScriptEnginePtr();
			MonoObject* safeObj = monoEngine->GetMonoBehaviorFromCS(entity->GetECSGroup()->GetGroupName(), entity->GetId(), script.scriptName);


			if (safeObj) {
				MonoClass* monoClass = mono_object_get_class(safeObj);
				MonoClass* serializeFieldClass = mono_class_from_name(mono_class_get_image(monoClass), "", "SerializeField");
				MonoClassField* field = nullptr;
				void* iter = nullptr;

				while ((field = mono_class_get_fields(monoClass, &iter))) {
					const char* fieldName = mono_field_get_name(field);

					MonoCustomAttrInfo* attrs = mono_custom_attrs_from_field(monoClass, field);
					if (attrs && mono_custom_attrs_has_attr(attrs, serializeFieldClass)) {
						// 値の取得
						MonoType* fieldType = mono_field_get_type(field);
						int type = mono_type_get_type(fieldType);

						ShowFiled(type, safeObj, field, fieldName);
					}
				}
			}

		}


		/// スクリプトが2種類以上ないと入れ替える意味がない
		if (scriptList.size() > 2) {

			/// スクリプトの順番を入れ替える処理
			// ---- ドラッグソース ----
			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("ScriptData", &i, sizeof(int)); // i番目のインデックスを送る
				ImGui::Text("script name : %s", scriptList[i].scriptName.c_str());
				ImGui::EndDragDropSource();
			}

			// ---- ドラッグターゲット ----
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ScriptData")) {
					int srcIndex = *(const int*)payload->Data;
					if (srcIndex != i) {
						std::swap(scriptList[srcIndex], scriptList[i]); // 要素の入れ替え
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		/// popupでスクリプトの削除などの操作を行う
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			ImGui::OpenPopup("##open");
		}

		/// 右クリックしたときのメニューの表示
		if (ImGui::BeginPopupContextItem("##popup")) {
			if (ImGui::MenuItem("delete")) {
				_script->RemoveScript(script.scriptName);
			}

			ImGui::EndPopup();
		}



		if (!script.enable) {
			ImGui::PopStyleColor(1);
		}

	}

	/// 現在のwindowのサイズを得る
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::InvisibleButton("##DropTarget", ImVec2(windowSize.x, 32.0f));
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			const char* droppedPath = static_cast<const char*>(payload->Data);
			std::string name = std::string(droppedPath);

			if (name.find(".cs") != std::string::npos) {
				/// フルパスをファイル名に変更
				size_t lastSlash = name.find_last_of("/\\");
				if (lastSlash != std::string::npos) {
					name = name.substr(lastSlash + 1);
				}

				/// .csを除去
				if (name.find(".cs") != std::string::npos) {
					name = name.substr(0, name.find(".cs"));
				}

				_script->AddScript(name);

				Console::Log(std::format("Script set to: {}", name));
			} else {
				Console::Log("Invalid script format. Please use .cs");
			}

		}
		ImGui::EndDragDropTarget();
	}


}

void MONO_INTERNAL_METHOD::InternalSetEnable(int32_t _entityId, MonoString* _scriptName, bool _enable, MonoString* _groupName) {
	/// Entityを取得
	std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	/// EntityからScriptコンポーネントを取得
	Script* script = entity->GetComponent<Script>();
	if (!script) {
		Console::LogError("Script component not found for Entity ID: " + std::to_string(_entityId));
	}

	/// スクリプト名をUTF-8に変換
	std::string scriptName = mono_string_to_utf8(_scriptName);
	/// スクリプトを有効/無効に設定
	script->SetEnable(scriptName, _enable);
	Console::Log(std::format("Script {} set to {} for Entity ID: {}", scriptName, _enable ? "enabled" : "disabled", _entityId));

}

bool MONO_INTERNAL_METHOD::InternalGetEnable(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName) {

	std::string groupName = mono_string_to_utf8(_groupName);

	/// Entityを取得
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return false;
	}

	/// EntityからScriptコンポーネントを取得
	Script* script = entity->GetComponent<Script>();
	if (!script) {
		Console::LogError("Script component not found for Entity ID: " + std::to_string(_entityId));
		return false;
	}

	/// スクリプト名をUTF-8に変換
	char* cstr = mono_string_to_utf8(_scriptName);
	std::string scriptName(cstr);
	/// スクリプトの有効/無効を取得
	bool isEnabled = script->GetEnable(scriptName);
	Console::Log(std::format("Script {} is {} for Entity ID: {}", scriptName, isEnabled ? "enabled" : "disabled", _entityId));

	mono_free(cstr);

	return isEnabled;
}

