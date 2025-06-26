#include "Script.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Script/MonoScriptEngine.h"


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

void COMP_DEBUG::ScriptDebug(Script* _script) {
	if (!_script) {
		return;
	}


	std::string ptrLable;
	std::vector<Script::ScriptData>& scriptList = _script->GetScriptDataList();
	for (auto& script : scriptList) {
		ptrLable = "##" + std::to_string(reinterpret_cast<uintptr_t>(&script));
	
		ImGui::Spacing();
		ImGui::SameLine();
		
		/// 有効/無効のチェックボックス
		ImGui::Checkbox(ptrLable.c_str(), &script.enable);

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();



		/// 有効/無効に応じてテキストの色を変える
		if (!script.enable) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		}

		ImGui::InputText((ptrLable + "name").c_str(), script.scriptName.data(), ImGuiInputTextFlags_ReadOnly);

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
