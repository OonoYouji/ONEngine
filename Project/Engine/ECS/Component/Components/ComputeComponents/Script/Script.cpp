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



	/// �X�N���v�g�̃��X�g��\��
	std::string ptrLable;
	std::vector<Script::ScriptData>& scriptList = _script->GetScriptDataList();
	for (auto& script : scriptList) {
		ptrLable = "##" + std::to_string(reinterpret_cast<uintptr_t>(&script));

		/// 
		ImGui::Checkbox(ptrLable.c_str(), &script.enable);

		/// ���ɕ\������e�L�X�g�̐F��ύX����
		if (script.enable) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		}

		ImGui::InputText((ptrLable + "name").c_str(), script.scriptName.data(), ImGuiInputTextFlags_ReadOnly);

		if (script.enable) {
			ImGui::PopStyleColor(ImGuiCol_Text);
		}


	}



}
