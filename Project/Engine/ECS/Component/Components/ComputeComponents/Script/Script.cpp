#include "Script.h"


/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"

/// editor
#include "Engine/Editor/Math/AssetPayload.h"
#include "Engine/Editor/Math/ImGuiShowField.h"

using namespace ONEngine;
using namespace Editor::CSGui;


/**
 * @brief スクリプトがC#インスタンスとして有効化されているかを取得します。
 */
bool Script::ScriptData::GetEnable(GameEntity* _entity) {
	MonoScriptEngine& monoEngine = MonoScriptEngine::GetInstance();
	MonoObject* obj = monoEngine.GetMonoBehaviorFromCS(
		_entity->GetECSGroup()->GetGroupName(), _entity->GetId(), scriptName
	);

	if(!obj) {
		return enable;
	}

	MonoClass* monoBehaviorClass = mono_object_get_class(obj);
	MonoClassField* field = mono_class_get_field_from_name(monoBehaviorClass, "enable");

	mono_field_get_value(obj, field, &enable);
	return enable;
}

/**
 * @brief スクリプトのC#インスタンスとしての有効/無効を設定します。
 */
void Script::ScriptData::SetEnable(GameEntity* _entity, bool _enable) {
	MonoScriptEngine& monoEngine = MonoScriptEngine::GetInstance();
	MonoObject* obj = monoEngine.GetMonoBehaviorFromCS(
		_entity->GetECSGroup()->GetGroupName(), _entity->GetId(), scriptName
	);

	if(!obj) {
		enable = _enable;
		return;
	}

	MonoClass* monoBehaviorClass = mono_object_get_class(obj);
	MonoClassField* field = mono_class_get_field_from_name(monoBehaviorClass, "enable");

	// 値を設定
	mono_field_set_value(obj, field, &_enable);
	enable = _enable;
}



/**
 * @brief コンストラクタ
 */
Script::Script() {
	SetIsAdded(false);
}

/**
 * @brief デストラクタ
 */
Script::~Script() {}

/**
 * @brief スクリプト名（クラス名）を指定して、C#スクリプトを新規に登録・アタッチします。
 */
void Script::AddScript(const std::string& _scriptName) {
	/// すでにアタッチされているかチェック
	if(scriptIndexMap_.contains(_scriptName)) {
		return;
	}

	ScriptData newScriptData;
	newScriptData.scriptName = _scriptName;

	/// インデックスを登録
	scriptIndexMap_[_scriptName] = scriptDataList_.size();
	scriptDataList_.push_back(std::move(newScriptData));
}

/**
 * @brief 指定された名前のC#スクリプトが登録済みかを判定します。
 */
bool Script::Contains(const std::string& _scriptName) const {
	if(scriptIndexMap_.contains(_scriptName)) {
		return true;
	}

	return false;
}

/**
 * @brief 指定されたスクリプトをデタッチ・削除します。
 */
void Script::RemoveScript(const std::string& _scriptName) {
	if(!scriptIndexMap_.contains(_scriptName)) {
		Console::LogWarning("Script " + _scriptName + " not found, cannot remove.");
		return;
	}

	size_t index = scriptIndexMap_[_scriptName];
	/// vectorの要素を削除するのに合わせてmapのインデックスも更新
	scriptIndexMap_.erase(_scriptName);
	for(auto& [name, value] : scriptIndexMap_) {
		if(value > index) {
			--value;  ///< 削除した分インデックスをずらす
		}
	}

	scriptDataList_.erase(scriptDataList_.begin() + index);
}

/**
 * @brief 登録インデックスからアタッチされているスクリプト名を取得します。
 */
const std::string& Script::GetScriptName(size_t _index) const {
	if(_index < scriptDataList_.size()) {
		return scriptDataList_[_index].scriptName;
	}

	Console::Log("Index out of range in GetScriptName");
	return scriptDataList_[0].scriptName;
}

/**
 * @brief 現在アタッチされている全スクリプト名のリストを取得します。
 */
std::vector<std::string> Script::GetScriptNames() const {
	std::vector<std::string> scriptNames;
	for(auto& script : scriptDataList_) {
		scriptNames.push_back(script.scriptName);
	}

	return scriptNames;
}

/**
 * @brief アタッチされているC#スクリプトデータ（メタ情報）の読み取り専用リストを取得します。
 */
const std::vector<Script::ScriptData>& Script::GetScriptDataList() const {
	return scriptDataList_;
}

/**
 * @brief アタッチされているC#スクリプトデータ（メタ情報）のリストを取得します。
 */
std::vector<Script::ScriptData>& Script::GetScriptDataList() {
	return scriptDataList_;
}

/**
 * @brief 指定したスクリプト名に対応するメタデータへのポインタを取得します（無ければ nullptr）。
 */
Script::ScriptData* Script::GetScriptData(const std::string& _scriptName) {
	for(auto& data : scriptDataList_) {
		if(data.scriptName == _scriptName) {
			return &data;  ///< 一致するスクリプトデータを返す
		}
	}

	Console::LogError("Script::GetScriptData - Script " + _scriptName + " not found in GetScriptData.");
	return nullptr;
}

/**
 * @brief 指定したスクリプト名に対応するC#インスタンスの有効/無効を設定します。
 */
void Script::SetEnable(const std::string& _scriptName, bool _enable) {
	/// スクリプト名が一致するものを探す
	for(auto& script : scriptDataList_) {
		if(script.scriptName == _scriptName) {
			script.SetEnable(GetOwner(), _enable);
			Console::Log("Script " + _scriptName + " enable set to " + std::to_string(_enable));
			return;
		}
	}


	/// 見つからなかった場合
	Console::LogWarning("Script " + _scriptName + " not found, cannot set enable state.");
}

/**
 * @brief 指定したスクリプト名に対応するC#インスタンスが有効であるかを取得します。
 */
bool Script::GetEnable(const std::string& _scriptName) {
	/// スクリプト名が一致するものを探す
	for(auto& script : scriptDataList_) {
		if(script.scriptName == _scriptName) {
			return script.GetEnable(GetOwner());
		}
	}

	/// 見つからなかった場合
	return false;
}

/**
 * @brief スクリプト側へエンティティのアタッチ登録が完了したかのフラグを設定します。
 */
void Script::SetIsAdded(bool _added) {
	isAdded_ = _added;
}

/**
 * @brief スクリプト側へエンティティのアタッチ登録が完了したかのフラグを取得します。
 */
bool Script::GetIsAdded() const {
	return isAdded_;
}

/**
 * @brief エディタ用：Scriptコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::ScriptDebug(Script* _script) {
	if(!_script) {
		return;
	}

	std::string ptrLable;
	std::vector<Script::ScriptData>& scriptList = _script->GetScriptDataList();
	std::string selectedScriptName;

	for(size_t i = 0; i < scriptList.size(); i++) {
		auto& script = scriptList[i];

		ptrLable = "##" + std::to_string(reinterpret_cast<uintptr_t>(&script));

		bool enable = script.GetEnable(_script->GetOwner());
		/// 有効/無効のチェックボックス
		if(ImGui::Checkbox(ptrLable.c_str(), &enable)) {
			script.SetEnable(_script->GetOwner(), enable);
		}

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		/// 有効/無効に応じてテキストの色を変える
		if(!enable) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		}


		if(ImGui::CollapsingHeader(script.scriptName.c_str())) {
			/// ------------------------------------------------------------------
			/// スクリプト内の[SerializeField]など表示
			/// ------------------------------------------------------------------

			GameEntity* entity = _script->GetOwner();
			MonoScriptEngine& monoEngine = MonoScriptEngine::GetInstance();
			MonoObject* safeObj = monoEngine.GetMonoBehaviorFromCS(entity->GetECSGroup()->GetGroupName(), entity->GetId(), script.scriptName);

			MonoClass* monoClass = nullptr;
			if(safeObj) {
				monoClass = mono_object_get_class(safeObj);
			} else if(monoEngine.Image()) {
				monoClass = mono_class_from_name(monoEngine.Image(), "", script.scriptName.c_str());
			}


			if(monoClass) {
				MonoClass* serializeFieldClass = mono_class_from_name(mono_class_get_image(monoClass), "", "SerializeField");
				MonoClassField* field = nullptr;
				void* iter = nullptr;

				while((field = mono_class_get_fields(monoClass, &iter))) {
					const char* fieldName = mono_field_get_name(field);

					MonoCustomAttrInfo* attrs = mono_custom_attrs_from_field(monoClass, field);
					if(attrs && mono_custom_attrs_has_attr(attrs, serializeFieldClass)) {
						// 値の取得
						MonoType* fieldType = mono_field_get_type(field);
						int type = mono_type_get_type(fieldType);

						Console::Log(std::format("[UndoDebug] Found [SerializeField] field: '{}' (type: {}) in script '{}'", fieldName, type, script.scriptName));

						if(safeObj) {
							ShowField(script.scriptName, type, safeObj, field, fieldName);
						} else {
							Variables* var = entity->GetComponent<Variables>();
							if (!var) {
								var = entity->AddComponent<Variables>();
								Console::Log(std::format("ScriptDebug: Added missing Variables component to entity '{}'.", entity->GetName()));
							}
							ShowFieldForVariables(var, script.scriptName, type, field, fieldName);
						}
					}
					if(attrs) mono_custom_attrs_free(attrs);
				}
			}

		}

		/// popupでスクリプトの削除などの操作を行う
		if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup(ptrLable.c_str());
			selectedScriptName = script.scriptName;
		}

		/// itemを左クリックしたときに出るメニュー
		if(ImGui::BeginPopup(ptrLable.c_str())) {
			if(ImGui::MenuItem("delete")) {
				_script->RemoveScript(script.scriptName);
			}
			ImGui::EndPopup();
		}


		/// スクリプトが2種類以上ないと入れ替える意味がない
		if(scriptList.size() > 2) {
			/// スクリプトの順番を入れ替える処理
			// ---- ドラッグソース ----
			if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("ScriptData", &i, sizeof(int)); // i番目のインデックスを送る
				ImGui::Text("script name : %s", scriptList[i].scriptName.c_str());
				ImGui::EndDragDropSource();
			}

			// ---- ドラッグターゲット ----
			if(ImGui::BeginDragDropTarget()) {
				if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ScriptData")) {
					int srcIndex = *(const int*)payload->Data;
					if(srcIndex != i) {
						std::swap(scriptList[srcIndex], scriptList[i]); // 要素の入れ替え
					}
				}
				ImGui::EndDragDropTarget();
			}
		}



		if(!enable) {
			ImGui::PopStyleColor(1);
		}
	}






	/// 現在のwindowのサイズを得る
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::InvisibleButton("##DropTarget", ImVec2(windowSize.x, 32.0f));
	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			if(payload->Data) {
				Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
				std::string name = assetPayload->filePath;

				if(name.find(".cs") != std::string::npos) {
					/// フルパスをファイル名に変更
					size_t lastSlash = name.find_last_of("/\\");
					if(lastSlash != std::string::npos) {
						name = name.substr(lastSlash + 1);
					}

					/// .csを除去
					if(name.find(".cs") != std::string::npos) {
						name = name.substr(0, name.find(".cs"));
					}

					_script->AddScript(name);

					Console::Log(std::format("Script set to: {}", name));
				} else {
					Console::Log("Invalid script format. Please use .cs");
				}

			}
		}
		ImGui::EndDragDropTarget();
	}


}

/**
 * @brief JSONからのデシリアライズ
 */
void ONEngine::from_json(const nlohmann::json& _j, Script& _s) {
	_s.enable = _j.at("enable").get<int>();
	if(_j.contains("scripts")) {
		nlohmann::json scriptJsons = _j.at("scripts");
		for(auto& data : scriptJsons) {
			if(data.contains("name")) {
				std::string scriptName = data.at("name").get<std::string>();
				_s.AddScript(scriptName);
				Script::ScriptData* scriptData = _s.GetScriptData(scriptName);
				if(scriptData) {
					if(data.contains("enable")) {
						scriptData->enable = data.at("enable").get<int>();
					}
				}
			} else {
				Console::LogError("Script component JSON does not contain 'name' in one of the scripts.");
			}
		}

	} else {
		Console::LogError("Script component JSON does not contain 'scripts'.");
	}
}

/**
 * @brief JSONへのシリアライズ
 */
void ONEngine::to_json(nlohmann::json& _j, const Script& _s) {
	nlohmann::json scriptJsons;
	for(auto& scriptData : _s.GetScriptDataList()) {
		nlohmann::json data{
			{ "enable", scriptData.enable },
			{ "name", scriptData.scriptName }
		};
		scriptJsons.push_back(data);
	}

	_j = nlohmann::json{
		{ "type", "Script" },
		{ "enable", _s.enable },
		{ "scripts", scriptJsons }
	};
}
