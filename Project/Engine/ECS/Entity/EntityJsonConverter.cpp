#include "EntityJsonConverter.h"

/// engine
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Collection/EntityCollection.h"
#include "Engine/ECS/Entity/Prefab/EntityPrefab.h"

using namespace ONEngine;

namespace {
	// コンポーネントのリストから指定したタイプのコンポーネントJSONを探す
	nlohmann::json FindComponentByType(const nlohmann::json& _components, const std::string& _type) {
		if (!_components.is_array()) return nlohmann::json();
		for (const auto& comp : _components) {
			if (comp.value("type", "") == _type) {
				return comp;
			}
		}
		return nlohmann::json();
	}

	// 数値の微小な誤差を考慮した比較
	bool IsJsonValueEqual(const nlohmann::json& a, const nlohmann::json& b) {
		if (a.is_number() && b.is_number()) {
			return std::abs(a.get<double>() - b.get<double>()) < 1e-4;
		}
		return a == b;
	}

	// 2つのJSONオブジェクトの差異を抽出する（再帰的）
	nlohmann::json GetJsonDiff(const nlohmann::json& _current, const nlohmann::json& _base) {
		if (IsJsonValueEqual(_current, _base)) return nlohmann::json();
		if (!_current.is_object() || !_base.is_object()) return _current;

		nlohmann::json diff = nlohmann::json::object();
		for (auto it = _current.begin(); it != _current.end(); ++it) {
			const std::string& key = it.key();
			// typeは常に含める（識別のため）
			if (key == "type") {
				diff[key] = it.value();
				continue;
			}

			if (!_base.contains(key)) {
				diff[key] = it.value();
			} else if (!IsJsonValueEqual(it.value(), _base[key])) {
				if (it.value().is_object() && _base[key].is_object()) {
					nlohmann::json subDiff = GetJsonDiff(it.value(), _base[key]);
					// type以外の実質的な差異がある場合のみ追加
					if (!subDiff.is_null() && subDiff.size() > (subDiff.contains("type") ? 1 : 0)) {
						diff[key] = subDiff;
					}
				} else {
					diff[key] = it.value();
				}
			}
		}
		return diff;
	}
// ... (MergeJson remains same)

	// パッチJSONをベースJSONにマージする（再帰的）
	void MergeJson(nlohmann::json& _base, const nlohmann::json& _patch) {
		if (!_patch.is_object() || !_base.is_object()) return;
		for (auto it = _patch.begin(); it != _patch.end(); ++it) {
			const std::string& key = it.key();
			if (it.value().is_object() && _base.contains(key) && _base[key].is_object()) {
				MergeJson(_base[key], it.value());
			} else {
				_base[key] = it.value();
			}
		}
	}
}

/**
 * @brief ゲームエンティティの構成データ（トランスフォーム、アタッチされたコンポーネント、子エンティティなど）をJSONオブジェクトへシリアライズします。
 */
nlohmann::json EntityJsonConverter::ToJson(const GameEntity* _entity, bool _forceFull) {
	if (!_entity) {
		return nlohmann::json();
	}
	nlohmann::json entityJson = nlohmann::json::object();
	entityJson["prefabName"] = _entity->GetPrefabName();
	entityJson["name"] = _entity->GetName();
	entityJson["active"] = _entity->active;
	// entityJson["id"] = _entity->GetId(); // DEPRECATED: id is now runtime-only
	entityJson["guid"] = _entity->GetGuid().ToString();

	// Prefabがある場合は差分のみを書き出す (forceFullがfalseの場合のみ)
	nlohmann::json prefabComponents;
	bool hasPrefab = false;
	if (!_forceFull && _entity->GetPrefabName() != "") {
		auto* collection = _entity->GetECSGroup()->GetEntityCollection();
		auto* prefab = collection->GetPrefab(_entity->GetPrefabName());
		if (prefab) {
			prefabComponents = prefab->GetJson().value("components", nlohmann::json::array());
			hasPrefab = true;
		}
	}

	// コンポーネントの情報を追加
	auto& components = _entity->GetComponents();
	for (const auto& component : components) {
		nlohmann::json compJson = ComponentJsonConverter::ToJson(component.second);
		if (compJson.empty()) continue;

		if (hasPrefab && !_forceFull) {
			std::string type = compJson.value("type", "");
			nlohmann::json prefabComp = FindComponentByType(prefabComponents, type);

			if (prefabComp.empty()) {
				// Prefabにないコンポーネントはそのまま保存
				entityJson["components"].push_back(compJson);
			} else {
				// 差分のみを計算
				nlohmann::json diff = GetJsonDiff(compJson, prefabComp);
				// type以外に変更があれば保存
				if (diff.size() > 1) {
					entityJson["components"].push_back(diff);
				}
			}
		} else {
			entityJson["components"].push_back(compJson);
		}
	}

	/// 親子関係の情報を追加
	if (_entity->GetParent()) {
		entityJson["parentGuid"] = _entity->GetParent()->GetGuid().ToString();
	} else {
		entityJson["parentGuid"] = nullptr;
	}

	return entityJson;
}

/**
 * @brief シリアライズされたJSONデータからゲームエンティティの構成要素（名前、有効状態、コンポーネントパラメータ等）をデシリアライズして復元・マージします。
 */
void EntityJsonConverter::FromJson(const nlohmann::json& _json, GameEntity* _entity, const std::string& /*_groupName*/, bool _merge) {

	/// name, prefabNameを設定
	if (_json.contains("name")) {
		_entity->SetName(_json.at("name").get<std::string>());
	}

	if (_json.contains("prefabName")) {
		const std::string& prefabName = _json.at("prefabName").get<std::string>();
		if (prefabName != "") {
			_entity->SetPrefabName(prefabName);
		}
	}

	if (_json.contains("active")) {
		_entity->active = _json.at("active").get<bool>();
	}

	/// コンポーネントを追加
	if (_json.contains("components")) {
		for (const auto& componentJson : _json["components"]) {

			/// jsonにtypeが無ければスキップ
			if (!componentJson.contains("type")) {
				continue;
			}

			const std::string componentType = componentJson.at("type").get<std::string>();

			Console::Log(std::format("[CPP ECS] FromJson - Adding component '{}' to entity '{}'", componentType, _entity->GetName()));

			IComponent* comp = _entity->AddComponent(componentType);
			if (comp) {
				if (_merge) {
					// 現在の状態をベースにマージして適用
					nlohmann::json base = ComponentJsonConverter::ToJson(comp);
					MergeJson(base, componentJson);
					ComponentJsonConverter::FromJson(base, comp);
				} else {
					// 直接上書き
					ComponentJsonConverter::FromJson(componentJson, comp);
				}
				comp->SetOwner(_entity);
			} else {
				// コンポーネントの追加に失敗した場合のログ
				Console::LogError("failed add component: " + componentType);
			}
		}
	}
}

/**
 * @brief ゲームエンティティのTransformコンポーネントに限定してJSONデータからトランスフォームパラメータ（位置、回転、縮尺等）を復元適用します。
 */
void EntityJsonConverter::TransformFromJson(const nlohmann::json& _json, GameEntity* _entity) {
	/// transformだけjsonから読み込む

	/// コンポーネントを追加
	if (_json.contains("components")) {
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
				Console::LogError("failed add component: " + componentType);
			}
		}
	}
}
