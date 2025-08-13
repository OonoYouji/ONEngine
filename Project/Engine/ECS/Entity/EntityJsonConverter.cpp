#include "EntityJsonConverter.h"

/// engine
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

nlohmann::json EntityJsonConverter::ToJson(const GameEntity* _entity) {
	if (!_entity) {
		return nlohmann::json();
	}
	nlohmann::json entityJson = nlohmann::json::object();
	entityJson["prefabName"] = _entity->GetPrefabName();
	entityJson["name"] = _entity->GetName();
	entityJson["id"] = _entity->GetId();

	// コンポーネントの情報を追加
	auto& components = _entity->GetComponents();
	for (const auto& component : components) {
		entityJson["components"].push_back(ComponentJsonConverter::ToJson(component.second));
	}

	/// 親子関係の情報を追加
	if (_entity->GetParent()) {
		entityJson["parent"] = _entity->GetParent()->GetId();
	} else {
		entityJson["parent"] = nullptr;
	}

	return entityJson;
}

void EntityJsonConverter::FromJson(const nlohmann::json& _json, GameEntity* _entity) {

	/// コンポーネントを追加
	for (const auto& componentJson : _json["components"]) {

		/// jsonにtypeが無ければスキップ
		if (!componentJson.contains("type")) {
			continue;
		}

		const std::string componentType = componentJson.at("type").get<std::string>();

		IComponent* comp = _entity->AddComponent(componentType);
		if (comp) {
			ComponentJsonConverter::FromJson(componentJson, comp);
			comp->SetOwner(_entity);

			if (componentType == "Variables") {
				Variables* vars = static_cast<Variables*>(comp);
				vars->LoadJson("./Assets/Jsons/" + _entity->GetName() + ".json");

			} else if (componentType == "Script") {
				Script* script = static_cast<Script*>(comp);
				script->ResetScripts(); 
			}

		} else {
			// コンポーネントの追加に失敗した場合のログ
			Console::Log("[error] failed add component: " + componentType);
		}
	}
}

void EntityJsonConverter::TransformFromJson(const nlohmann::json& _json, GameEntity* _entity) {
	/// transformだけjsonから読み込む

	/// コンポーネントを追加
	for (const auto& componentJson : _json["components"]) {
		/// jsonにtypeが無ければスキップ
		if (!componentJson.contains("type")) {
			continue;
		}

		const std::string componentType = componentJson.at("type").get<std::string>();
		if (componentType != "Transform") {
			continue; // Transformコンポーネント以外はスキップ
		}


		IComponent* comp = _entity->AddComponent(componentType);
		if (comp) {
			ComponentJsonConverter::FromJson(componentJson, comp);
			comp->SetOwner(_entity);
		} else {
			// コンポーネントの追加に失敗した場合のログ
			Console::Log("[error] failed add component: " + componentType);
		}
	}
}
