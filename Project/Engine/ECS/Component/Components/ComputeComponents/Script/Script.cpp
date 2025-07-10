#include "Script.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiShowField.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"

using namespace CSGui;



Script::Script() {}
Script::~Script() {}

void Script::AddScript(const std::string& _scriptName) {
	/// すでにアタッチされているかチェック
	for (auto& script : scriptDataList_) {
		if (script.scriptName == _scriptName) {
			Console::Log("Script is already attached: " + _scriptName);
			return;  ///< すでにアタッチされているので何もしない
		}
	}

	ScriptData scriptData;
	GetMonoScriptEnginePtr()->MakeScript(this, &scriptData, _scriptName);
	scriptDataList_.push_back(std::move(scriptData));
}

void Script::RemoveScript(const std::string& _scriptName) {
	/// スクリプト名が一致するものを探す
	for (auto itr = scriptDataList_.begin(); itr != scriptDataList_.end(); ) {

		ScriptData* data = &(*itr);
		if (data->scriptName == _scriptName) {
			/// GCハンドルを解放
			if (data->gcHandle != 0) {
				mono_gchandle_free(data->gcHandle);
				data->gcHandle = 0;
			}
			data->instance = nullptr;
			data->monoClass = nullptr;
			data->initMethod = nullptr;
			data->updateMethod = nullptr;
			Console::Log("Script removed: " + _scriptName);

			itr = scriptDataList_.erase(itr);  ///< 削除してイテレータを更新
		} else {
			++itr;  ///< 次の要素へ
		}
	}

}

void Script::ResetScripts() {
	for (auto& script : scriptDataList_) {
		GetMonoScriptEnginePtr()->MakeScript(this, &script, script.scriptName);
	}
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

void Script::CallAwakeMethodAll() {
	Console::Log("Script::CallAwakeMethodAll called, owner:\"" + GetOwner()->GetName() + "\"");

	for (auto& script : scriptDataList_) {
		if (!script.enable) {
			Console::Log("Script::CallAwakeMethodAll Script is disabled");
			continue;
		}

		///< Initメソッドは一度だけ呼び出す
		if (script.isCalledAwake) {
			continue;
		} else {
			script.isCalledAwake = true;
		}

		if (!script.internalInitMethod || !script.instance) {
			Console::LogError("Script::CallAwakeMethodAll Script is invalid or has no internalInitMethod");
			continue;
		}


		/// 関数を呼び出す
		/// 引数の準備、(entity id)
		void* args[1];
		int32_t entityId = static_cast<int32_t>(GetOwner()->GetId());
		args[0] = &entityId;

		/// 例外のチェック用
		MonoObject* exc = nullptr;

		/// 実行
		mono_runtime_invoke(script.internalInitMethod, script.instance, args, &exc);

		/// 例外が発生したら処理
		if (exc) {
			char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
			Console::LogWarning(std::string("Exception thrown in Awake: ") + err);
			mono_free(err);
		}
	}

}

void Script::CallInitMethodAll() {
	Console::Log("Script::CallInitMethodAll called, owner:\"" + GetOwner()->GetName() + "\"");

	for (auto& script : scriptDataList_) {
		if (!script.enable) {
			Console::Log("Script::CallInitMethodAll Script is disabled");
			continue;
		}

		///< Initメソッドは一度だけ呼び出す
		if (script.isCalledInit) {
			continue;
		} else {
			script.isCalledInit = true;
		}

		if (!script.initMethod || !script.instance) {
			Console::LogError("Script::CallInitMethodAll Script is invalid or has no initMethod");
			continue;
		}

		/// 関数を呼び出す
		/// 例外のチェック用
		MonoObject* exc = nullptr;

		/// 実行
		mono_runtime_invoke(script.initMethod, script.instance, nullptr, &exc);

		/// 例外が発生したら処理
		if (exc) {
			char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
			Console::LogWarning(std::string("Exception thrown in Init: ") + err);
			mono_free(err);
		}

	}

}

void Script::CallUpdateMethodAll() {
	Console::Log("Script::CallUpdateMethodAll called, owner:\"" + GetOwner()->GetName() + "\"");

	for (auto& script : scriptDataList_) {
		if (!script.enable) {
			Console::Log("Script::CallUpdateMethodAll Script is disabled");
			continue;
		}

		if (!script.updateMethod || !script.instance) {
			Console::LogError("Script::CallUpdateMethodAll Script is invalid or has no updateMethod");
			continue;
		}

		const char* methodName = mono_method_get_name(script.updateMethod);
		const char* className = mono_class_get_name(mono_method_get_class(script.updateMethod));
		Console::Log("Method found: " + std::string(className) + "::" + methodName);


		/// 関数を呼び出す
		/// 例外のチェック用
		MonoObject* exc = nullptr;

		/// 実行
		mono_runtime_invoke(script.updateMethod, script.instance, nullptr, &exc);

		/// 例外が発生したら処理
		if (exc) {
			char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
			Console::LogWarning(std::string("Exception thrown in Update: ") + err);
			mono_free(err);
		}

	}

}

void COMP_DEBUG::ScriptDebug(Script* _script) {
	if (!_script) {
		return;
	}


	std::string ptrLable;
	std::vector<Script::ScriptData>& scriptList = _script->GetScriptDataList();
	for (auto& script : scriptList) {
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
			MonoClass* monoClass = mono_object_get_class(script.instance);
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

					ShowFiled(type, script.instance, field, fieldName);
				}

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
