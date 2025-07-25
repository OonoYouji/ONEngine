#include "EntityJsonConverter.h"

/// engine
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

nlohmann::json EntityJsonConverter::ToJson(const IEntity* _entity) {
	if (!_entity) {
		return nlohmann::json();
	}
	nlohmann::json entityJson = nlohmann::json::object();
	entityJson["prefabName"] = _entity->GetPrefabName();
	entityJson["className"] = _entity->GetEntityClassName();
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

void EntityJsonConverter::FromJson(const nlohmann::json& _json, IEntity* _entity) {

	/// コンポーネントを追加
	for (const auto& componentJson : _json["components"]) {
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
			}

			if (componentType == "Script") {
				Script* script = static_cast<Script*>(comp);
				script->ResetScripts(); 
			}

		} else {
			// コンポーネントの追加に失敗した場合のログ
			Console::Log("[error] failed add component: " + componentType);
		}
	}
}
