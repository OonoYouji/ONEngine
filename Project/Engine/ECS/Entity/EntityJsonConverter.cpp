#include "EntityJsonConverter.h"

/// engine
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

nlohmann::json EntityJsonConverter::ToJson(const IEntity* _entity) {
	if (!_entity) {
		return nlohmann::json();
	}
	nlohmann::json entityJson = nlohmann::json::object();
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
