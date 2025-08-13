#include "Script.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiShowField.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

using namespace CSGui;



Script::Script() {}
Script::~Script() {
	ReleaseGCHandles();
}

void Script::AddScript(const std::string& _scriptName) {
	/// すでにアタッチされているかチェック
	for (auto& script : scriptDataList_) {
		if (script.scriptName == _scriptName) {

			/// 生成済みのGCHandleを解放
			ReleaseGCHandle(&script);

			/// アタッチされている場合は再生成する
			GetMonoScriptEnginePtr()->MakeScript(this, &script, _scriptName);
			return;
		}
	}

	ScriptData scriptData;
	GetMonoScriptEnginePtr()->MakeScript(this, &scriptData, _scriptName);
	scriptDataList_.push_back(std::move(scriptData));
}

bool Script::Contains(const std::string& _scriptName) const {
	for (const auto& sdata : scriptDataList_) {
		if (sdata.scriptName == _scriptName) {
			/// 同一のものを見つけた
			return true;
		}
	}

	return false;
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
	ReleaseGCHandles();
	for (auto& script : scriptDataList_) {
		GetMonoScriptEnginePtr()->MakeScript(this, &script, script.scriptName);
	}
}

void Script::ReleaseGCHandles() {
	for (auto& script : scriptDataList_) {
		ReleaseGCHandle(&script);
	}
}

void Script::ReleaseGCHandle(ScriptData* _releaseScript) {
	if (!_releaseScript) {
		Console::LogError("ScriptData pointer is null in ReleaseGCHandle");
		return;
	}

	if (_releaseScript->gcHandle != 0) {
		Console::LogInfo("released gcHandle [" + std::to_string(_releaseScript->gcHandle) + "]");
		mono_gchandle_free(_releaseScript->gcHandle);
		_releaseScript->gcHandle = 0;
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

void Script::CallAwakeMethodAll() {
	/// 生成処理は有効/無効関係なく呼び出す

	for (auto& script : scriptDataList_) {

		///< 生成メソッドは一度だけ呼び出す
		if (script.isCalledAwake) {
			continue;
		} else {
			script.isCalledAwake = true;
		}

		/// Variables Componentから値を取得して、スクリプトに適用する
		Variables* variables = GetOwner()->GetComponent<Variables>();
		if (variables) {
			variables->SetScriptVariables(script.scriptName);
		} else {
			Console::LogWarning("Script::CallAwakeMethodAll Variables component not found.");
		}

		MonoObject* safeObj = nullptr;
		if (script.gcHandle != 0) {
			safeObj = mono_gchandle_get_target(script.gcHandle);
		}

		if (!script.internalInitMethod || !safeObj) {
			Console::LogError(
				"Script::CallAwakeMethodAll Script is invalid or has no internalInitMethod. owner:" + GetOwner()->GetName() + "\", " +
				"script name:\"" + script.scriptName + "\""
			);
			continue;
		}

		/// ログに出す
		Console::Log(
			"called script awake, owner:\"" + GetOwner()->GetName() + "\", " +
			"script name:\"" + script.scriptName + "\""
		);

		/// 関数を呼び出す
		/// 引数の準備、(entity id)
		void* args[2];
		int32_t entityId = static_cast<int32_t>(GetOwner()->GetId());
		args[0] = &entityId;
		args[1] = mono_string_new(mono_domain_get(), script.scriptName.c_str());

		/// 実行。 exc:例外のチェック用
		MonoObject* exc = nullptr;
		mono_runtime_invoke(script.internalInitMethod, safeObj, args, &exc);

		/// 例外が発生したら処理
		if (exc) {
			char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
			Console::LogWarning(std::string("Exception thrown in Awake: ") + err);
			mono_free(err);
		}
	}

}

void Script::CallInitMethodAll() {
	/// 初期化は有効/無効関係なく呼び出す

	for (auto& script : scriptDataList_) {

		///< Initメソッドは一度だけ呼び出す
		if (script.isCalledInit) {
			continue;
		} else {
			script.isCalledInit = true;
		}

		MonoObject* safeObj = nullptr;
		if (script.gcHandle != 0) {
			safeObj = mono_gchandle_get_target(script.gcHandle);
		}

		if (!script.initMethod || !safeObj) {
			Console::LogError("Script::CallInitMethodAll Script is invalid or has no initMethod");
			continue;
		}

		/// ログに出す
		Console::Log(
			"called script initialize, owner:\"" + GetOwner()->GetName() + "\", " +
			"script name:\"" + script.scriptName + "\""
		);

		/// 関数を呼び出す。 exc:例外のチェック用
		MonoObject* exc = nullptr;
		mono_runtime_invoke(script.initMethod, safeObj, nullptr, &exc);


		/// 例外が発生したら処理
		if (exc) {
			char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
			Console::LogWarning(std::string("Exception thrown in Init: ") + err);
			mono_free(err);
		}

	}

}

void Script::CallUpdateMethodAll() {

	for (auto& script : scriptDataList_) {
		/// 初期化済みか確認
		if (!script.isCalledAwake || !script.isCalledInit) {
			continue;
		}


		if (!script.enable) {
			Console::Log("Script::CallUpdateMethodAll Script is disabled");
			continue;
		}

		/// gcHandle経由でMonoObjectを取得
		MonoObject* safeObj = nullptr;
		if (script.gcHandle != 0) {
			safeObj = mono_gchandle_get_target(script.gcHandle);
		}

		if (!script.updateMethod || !safeObj) {
			Console::LogError("Script::CallUpdateMethodAll Script is invalid or has no updateMethod");
			continue;
		}

		/// ログに出す
		Console::Log(
			"called script update, owner:\"" + GetOwner()->GetName() + "\", " +
			"script name:\"" + script.scriptName + "\""
		);


		/// 関数を呼び出す。 exc:例外のチェック用
		MonoObject* exc = nullptr;
		mono_runtime_invoke(script.updateMethod, safeObj, nullptr, &exc);

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

			MonoObject* safeObj = nullptr;
			if (script.gcHandle != 0) {
				safeObj = mono_gchandle_get_target(script.gcHandle);
			}

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

void MONO_INTENRAL_METHOD::InternalSetEnable(int32_t _entityId, MonoString* _scriptName, bool _enable) {
	/// Entityを取得
	GameEntity* entity = GetEntityById(_entityId);
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

bool MONO_INTENRAL_METHOD::InternalGetEnable(int32_t _entityId, MonoString* _scriptName) {
	/// Entityを取得
	GameEntity* entity = GetEntityById(_entityId);
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

