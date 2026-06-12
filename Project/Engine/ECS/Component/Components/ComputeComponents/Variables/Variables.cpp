#include "Variables.h"

/// std
#include <fstream>
#include <filesystem>
#include <format>
#include <vector>
#include <variant>

/// external
#include <imgui.h>
#include <Externals/imgui/dialog/ImGuiFileDialog.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/blob.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>

/// engine
#include "Engine/Core/Utility/Math/Math.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Script/MonoScriptEngine.h"

/// editor 
#include "Engine/Editor/Math/ImGuiMath.h"

using namespace ONEngine;
using json = nlohmann::json;

namespace {

	bool IsVectorN(const json& j, int n) {
		if (!j.is_object()) {
			return false;
		}

		static const char* keys[] = { "x", "y", "z", "w" };
		for (int i = 0; i < n; ++i) {
			if (!j.contains(keys[i]) || !j[keys[i]].is_number()) {
				return false;
			}
		}
		return true;
	}

	bool HasSerializeField(MonoClassField* field) {
		MonoClass* klass = mono_field_get_parent(field);
		MonoCustomAttrInfo* attrs = mono_custom_attrs_from_field(klass, field);
		if (!attrs) return false;

		static MonoClass* serializeFieldAttr = nullptr;
		if (!serializeFieldAttr) {
			serializeFieldAttr = mono_class_from_name(MonoScriptEngine::GetInstance().Image(), "", "SerializeField");
		}

		bool has = serializeFieldAttr && mono_custom_attrs_has_attr(attrs, serializeFieldAttr);
		mono_custom_attrs_free(attrs);
		return has;
	}

	bool IsPublicField(MonoClassField* field) {
		uint32_t flags = mono_field_get_flags(field);
		return (flags & 0x0006) == 0x0006; // FIELD_ATTRIBUTE_PUBLIC
	}

	bool ShouldSerialize(MonoClassField* field) {
		return IsPublicField(field) || HasSerializeField(field);
	}

	Variables::Var JsonToVar(const json& varValue);
	json VarToJson(const Variables::Var& var);

	std::shared_ptr<Variables::GenericObject> JsonToObject(const json& j) {
		auto obj = std::make_shared<Variables::GenericObject>();
		if (j.contains("__type")) obj->typeName = j["__type"];
		for (auto& [k, v] : j.items()) {
			if (k == "__type") continue;
			obj->fields[k] = JsonToVar(v);
		}
		return obj;
	}

	Variables::Var JsonToVar(const json& varValue) {
		if (varValue.is_array()) {
			if (varValue.empty()) {
				return std::vector<int>();
			}
			if (varValue[0].is_object() && !IsVectorN(varValue[0], 2)) {
				std::vector<std::shared_ptr<Variables::GenericObject>> list;
				for (const auto& v : varValue) list.push_back(JsonToObject(v));
				return list;
			}
			if (varValue[0].is_number_integer()) return varValue.get<std::vector<int>>();
			if (varValue[0].is_number_float()) return varValue.get<std::vector<float>>();
			if (varValue[0].is_boolean()) return varValue.get<std::vector<bool>>();
			if (varValue[0].is_string()) return varValue.get<std::vector<std::string>>();
			if (IsVectorN(varValue[0], 3)) {
				std::vector<Vector3> vecs;
				for (const auto& v : varValue) vecs.push_back(v);
				return vecs;
			}
			return std::vector<int>();
		}
		if (varValue.is_object()) {
			if (IsVectorN(varValue, 4)) return (Vector4)varValue;
			if (IsVectorN(varValue, 3)) return (Vector3)varValue;
			if (IsVectorN(varValue, 2)) return (Vector2)varValue;
			return JsonToObject(varValue);
		}
		if (varValue.is_number_integer()) return varValue.get<int>();
		if (varValue.is_number_float()) return varValue.get<float>();
		if (varValue.is_boolean()) return varValue.get<bool>();
		if (varValue.is_string()) return varValue.get<std::string>();
		return 0;
	}

	json VarToJson(const Variables::Var& var) {
		return std::visit([](auto&& _arg) -> json {
			using T = std::decay_t<decltype(_arg)>;
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, bool> || std::is_same_v<T, std::string> ||
				std::is_same_v<T, Vector2> || std::is_same_v<T, Vector3> || std::is_same_v<T, Vector4> ||
				std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<float>> ||
				std::is_same_v<T, std::vector<bool>> || std::is_same_v<T, std::vector<std::string>>) {
				return _arg;
			} else if constexpr (std::is_same_v<T, std::vector<Vector3>>) {
				json j = json::array();
				for (const auto& v : _arg) j.push_back(v);
				return j;
			} else if constexpr (std::is_same_v<T, std::shared_ptr<Variables::GenericObject>>) {
				json j = json::object();
				if (_arg) {
					j["__type"] = _arg->typeName;
					for (auto& [k, v] : _arg->fields) j[k] = VarToJson(v);
				}
				return j;
			} else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Variables::GenericObject>>>) {
				json j = json::array();
				for (const auto& o : _arg) {
					json oj = json::object();
					if (o) {
						oj["__type"] = o->typeName;
						for (auto& [k, v] : o->fields) oj[k] = VarToJson(v);
					}
					j.push_back(oj);
				}
				return j;
			}
			return json();
			}, var);
	}

}	/// namespace

Variables::Var Variables::MonoObjectToVar(void* obj, void* type) {
	int typeId = mono_type_get_type((MonoType*)type);

	if (!obj) {
		switch (typeId) {
		case MONO_TYPE_I4: return 0;
		case MONO_TYPE_R4: return 0.0f;
		case MONO_TYPE_BOOLEAN: return false;
		case MONO_TYPE_STRING: return std::string("");
		case MONO_TYPE_VALUETYPE:
		{
			MonoClass* klass = mono_class_from_mono_type((MonoType*)type);
			const char* name = mono_class_get_name(klass);
			if (strcmp(name, "Vector2") == 0) return Vector2::Zero;
			if (strcmp(name, "Vector3") == 0) return Vector3::Zero;
			if (strcmp(name, "Vector4") == 0) return Vector4::Zero;
			if (mono_class_is_enum(klass)) return 0;
			auto gen = std::make_shared<Variables::GenericObject>();
			gen->typeName = name;
			void* iter = nullptr; MonoClassField* f;
			while ((f = mono_class_get_fields(klass, &iter))) {
				if (ShouldSerialize(f)) gen->fields[mono_field_get_name(f)] = MonoObjectToVar(nullptr, mono_field_get_type(f));
			}
			return gen;
		}
		case MONO_TYPE_CLASS: return std::shared_ptr<GenericObject>(nullptr);
		case MONO_TYPE_GENERICINST: return std::vector<int>();
		default: return 0;
		}
	}

	switch (typeId) {
	case MONO_TYPE_I4: return *(int*)mono_object_unbox((MonoObject*)obj);
	case MONO_TYPE_R4: return *(float*)mono_object_unbox((MonoObject*)obj);
	case MONO_TYPE_BOOLEAN: return *(bool*)mono_object_unbox((MonoObject*)obj);
	case MONO_TYPE_STRING: return mono_string_to_utf8((MonoString*)obj);
	case MONO_TYPE_VALUETYPE:
	{
		MonoClass* klass = mono_class_from_mono_type((MonoType*)type);
		const char* name = mono_class_get_name(klass);
		if (strcmp(name, "Vector2") == 0) return *(Vector2*)mono_object_unbox((MonoObject*)obj);
		if (strcmp(name, "Vector3") == 0) return *(Vector3*)mono_object_unbox((MonoObject*)obj);
		if (strcmp(name, "Vector4") == 0) return *(Vector4*)mono_object_unbox((MonoObject*)obj);
		if (mono_class_is_enum(klass)) return *(int*)mono_object_unbox((MonoObject*)obj);
		return MonoObjectToGeneric(obj);
	}
	case MONO_TYPE_CLASS: return MonoObjectToGeneric(obj);
	case MONO_TYPE_GENERICINST:
	{
		MonoClass* klass = mono_object_get_class((MonoObject*)obj);
		if (strcmp(mono_class_get_name(klass), "List`1") != 0) return 0;
		MonoMethod* getCount = mono_class_get_method_from_name(klass, "get_Count", 0);
		int count = *(int*)mono_object_unbox(mono_runtime_invoke(getCount, (MonoObject*)obj, nullptr, nullptr));
		MonoMethod* getItem = mono_class_get_method_from_name(klass, "get_Item", 1);
		MonoType* elemType = mono_signature_get_return_type(mono_method_signature(getItem));

		int etid = mono_type_get_type(elemType);
		if (etid == MONO_TYPE_I4) {
			std::vector<int> l(count);
			for (int i = 0; i < count; ++i) { void* a[1] = { &i }; l[i] = *(int*)mono_object_unbox(mono_runtime_invoke(getItem, (MonoObject*)obj, a, nullptr)); }
			return l;
		}
		if (etid == MONO_TYPE_R4) {
			std::vector<float> l(count);
			for (int i = 0; i < count; ++i) { void* a[1] = { &i }; l[i] = *(float*)mono_object_unbox(mono_runtime_invoke(getItem, (MonoObject*)obj, a, nullptr)); }
			return l;
		}
		if (etid == MONO_TYPE_BOOLEAN) {
			std::vector<bool> l(count);
			for (int i = 0; i < count; ++i) { void* a[1] = { &i }; l[i] = *(bool*)mono_object_unbox(mono_runtime_invoke(getItem, (MonoObject*)obj, a, nullptr)); }
			return l;
		}
		if (etid == MONO_TYPE_STRING) {
			std::vector<std::string> l(count);
			for (int i = 0; i < count; ++i) { void* a[1] = { &i }; MonoObject* s = mono_runtime_invoke(getItem, (MonoObject*)obj, a, nullptr); if (s) l[i] = mono_string_to_utf8((MonoString*)s); }
			return l;
		}
		if (etid == MONO_TYPE_VALUETYPE || etid == MONO_TYPE_CLASS) {
			MonoClass* ek = mono_class_from_mono_type(elemType);
			if (strcmp(mono_class_get_name(ek), "Vector3") == 0) {
				std::vector<Vector3> l(count);
				for (int i = 0; i < count; ++i) { void* a[1] = { &i }; l[i] = *(Vector3*)mono_object_unbox(mono_runtime_invoke(getItem, (MonoObject*)obj, a, nullptr)); }
				return l;
			}
			std::vector<std::shared_ptr<Variables::GenericObject>> l(count);
			for (int i = 0; i < count; ++i) { void* a[1] = { &i }; l[i] = MonoObjectToGeneric(mono_runtime_invoke(getItem, (MonoObject*)obj, a, nullptr)); }
			return l;
		}
	}
	}
	return 0;
}

std::shared_ptr<Variables::GenericObject> Variables::MonoObjectToGeneric(void* obj) {
	if (!obj) return nullptr;
	auto gen = std::make_shared<Variables::GenericObject>();
	MonoClass* klass = mono_object_get_class((MonoObject*)obj);
	gen->typeName = mono_class_get_name(klass);

	void* iter = nullptr;
	MonoClassField* field = nullptr;
	while ((field = mono_class_get_fields(klass, &iter))) {
		if (ShouldSerialize(field)) {
			const char* name = mono_field_get_name(field);
			MonoType* type = mono_field_get_type(field);
			gen->fields[name] = MonoObjectToVar(mono_field_get_value_object(mono_domain_get(), field, (MonoObject*)obj), type);
		}
	}
	return gen;
}

void Variables::VarToMonoObject(void* obj, void* klass, const Variables::Var& var) {
	if (!obj || !std::holds_alternative<std::shared_ptr<Variables::GenericObject>>(var)) return;
	auto gen = std::get<std::shared_ptr<Variables::GenericObject>>(var);
	if (!gen) return;

	for (auto& [name, val] : gen->fields) {
		MonoClassField* field = mono_class_get_field_from_name((MonoClass*)klass, name.c_str());
		if (!field) continue;

		std::visit([&](auto&& _arg) {
			using T = std::decay_t<decltype(_arg)>;
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, bool> || std::is_same_v<T, Vector2> || std::is_same_v<T, Vector3> || std::is_same_v<T, Vector4>) {
				mono_field_set_value((MonoObject*)obj, field, (void*)&_arg);
			} else if constexpr (std::is_same_v<T, std::string>) {
				MonoString* s = mono_string_new(mono_domain_get(), _arg.c_str());
				mono_field_set_value((MonoObject*)obj, field, s);
			} else if constexpr (std::is_same_v<T, std::shared_ptr<Variables::GenericObject>>) {
				MonoObject* child = mono_field_get_value_object(mono_domain_get(), field, (MonoObject*)obj);
				if (child) VarToMonoObject(child, mono_object_get_class(child), val);
			} else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Variables::GenericObject>>>) {
				MonoObject* list = mono_field_get_value_object(mono_domain_get(), field, (MonoObject*)obj);
				if (list) {
					MonoClass* lc = mono_object_get_class(list);
					MonoMethod* clear = mono_class_get_method_from_name(lc, "Clear", 0);
					mono_runtime_invoke(clear, list, nullptr, nullptr);
					MonoMethod* add = mono_class_get_method_from_name(lc, "Add", 1);
					MonoType* et = mono_signature_get_return_type(mono_method_signature(mono_class_get_method_from_name(lc, "get_Item", 1)));
					MonoClass* ek = mono_class_from_mono_type(et);
					for (auto& itemGen : _arg) {
						MonoObject* item = mono_object_new(mono_domain_get(), ek);
						mono_runtime_object_init(item);
						VarToMonoObject(item, ek, itemGen);
						void* args[1] = { item };
						mono_runtime_invoke(add, list, args, nullptr);
					}
				}
			}
			}, val);
	}
}

void ONEngine::from_json(const nlohmann::json& _j, Variables& _v) {
	_v.groupKeyMap_.clear();
	_v.groups_.clear();

	for (auto& [groupKey, groupValue] : _j.items()) {
		if (groupKey == "type") continue;

		if (!_v.HasGroup(groupKey)) _v.AddGroup(groupKey);
		Variables::Group& group = _v.groups_[_v.groupKeyMap_.at(groupKey)];

		for (auto& [varKey, varValue] : groupValue.items()) {
			group.Add(varKey, JsonToVar(varValue));
		}
	}
}

void ONEngine::to_json(nlohmann::json& _j, const Variables& _v) {
	_j = nlohmann::json::object();
	_j["type"] = "Variables";

	for (const auto& [groupKey, value] : _v.groupKeyMap_) {
		_j[groupKey] = nlohmann::json::object();
		for (const auto& [varKey, varValue] : _v.groups_[value].keyMap) {
			_j[groupKey][varKey] = VarToJson(_v.groups_[value].variables[varValue]);
		}
	}
}

Variables::Variables() {
	groupKeyMap_.clear();
	groups_.clear();
}

Variables::~Variables() = default;

void Variables::LoadJson(const std::string& _path) {
	std::string ext = FileSystem::FileExtension(_path);
	if (ext != ".json" && ext != ".entity") return;
	if (!std::filesystem::exists(_path)) return;

	nlohmann::json j;
	{
		std::ifstream ifs(_path);
		if (!ifs.is_open()) return;
		ifs >> j;
		ifs.close();
	}

	if (j.contains("components")) {
		for (const auto& compJson : j["components"]) {
			if (compJson.value("type", "") == "Variables") {
				from_json(compJson, *this);
				break;
			}
		}
	} else {
		from_json(j, *this);
	}

	if (Script* script = GetOwner()->GetComponent<Script>()) {
		for (const auto& data : script->GetScriptDataList()) {
			SetScriptVariables(data.scriptName);
		}
	}
}

void Variables::SaveJson(const std::string& _path) {
	nlohmann::json j;
	GameEntity* owner = GetOwner();
	if (!owner) return;

	to_json(j, *this);
	if (j.contains("type")) j.erase("type");

	std::filesystem::path path(_path);
	std::filesystem::create_directories(path.parent_path());

	std::ofstream ofs(_path);
	if (!ofs) throw std::runtime_error("Failed to open: " + _path);
	ofs << j.dump(4);
}

void Variables::RegisterScriptVariables() {
	Script* script = GetOwner()->GetComponent<Script>();
	if (!script) return;

	for (const auto& data : script->GetScriptDataList()) {
		size_t groupIndex = 0;
		if (!HasGroup(data.scriptName)) groupIndex = AddGroup(data.scriptName);
		else groupIndex = groupKeyMap_.at(data.scriptName);
		Group& group = groups_[groupIndex];

		{
			MonoScriptEngine& monoEngine = MonoScriptEngine::GetInstance();
			GameEntity* entity = GetOwner();
			MonoObject* safeObj = monoEngine.GetMonoBehaviorFromCS(entity->GetECSGroup()->GetGroupName(), entity->GetId(), data.scriptName);
			if (!safeObj) continue;

			MonoClass* monoClass = mono_object_get_class(safeObj);
			void* iter = nullptr;
			MonoClassField* field = nullptr;
			while ((field = mono_class_get_fields(monoClass, &iter))) {
				if (!ShouldSerialize(field)) continue;
				const char* fieldName = mono_field_get_name(field);
				if (group.Has(fieldName)) continue;
				group.Add(fieldName, MonoObjectToVar(mono_field_get_value_object(mono_domain_get(), field, safeObj), mono_field_get_type(field)));
			}
		}
	}
}

void Variables::ReloadScriptVariables() {
	Script* script = GetOwner()->GetComponent<Script>();
	if (!script) return;

	for (const auto& data : script->GetScriptDataList()) {
		size_t groupIndex = 0;
		if (!HasGroup(data.scriptName)) groupIndex = AddGroup(data.scriptName);
		else groupIndex = groupKeyMap_.at(data.scriptName);
		Group& group = groups_[groupIndex];

		{
			MonoScriptEngine& monoEngine = MonoScriptEngine::GetInstance();
			GameEntity* entity = GetOwner();
			MonoObject* safeObj = monoEngine.GetMonoBehaviorFromCS(entity->GetECSGroup()->GetGroupName(), entity->GetId(), data.scriptName);
			if (!safeObj) continue;

			MonoClass* monoClass = mono_object_get_class(safeObj);
			void* iter = nullptr;
			MonoClassField* field = nullptr;
			while ((field = mono_class_get_fields(monoClass, &iter))) {
				if (!ShouldSerialize(field)) continue;
				group.Add(mono_field_get_name(field), MonoObjectToVar(mono_field_get_value_object(mono_domain_get(), field, safeObj), mono_field_get_type(field)));
			}
		}
	}
}

void Variables::SetScriptVariables(const std::string& _scriptName) {
	GameEntity* owner = GetOwner();
	if (!owner) return;
	Script* script = owner->GetComponent<Script>();
	if (!script || !HasGroup(_scriptName)) return;

	Group& group = groups_[groupKeyMap_.at(_scriptName)];
	MonoScriptEngine& monoEngine = MonoScriptEngine::GetInstance();
	MonoObject* safeObj = monoEngine.GetMonoBehaviorFromCS(owner->GetECSGroup()->GetGroupName(), owner->GetId(), _scriptName);
	if (!safeObj) return;

	MonoClass* monoClass = mono_object_get_class(safeObj);
	void* iter = nullptr;
	MonoClassField* field = nullptr;
	while ((field = mono_class_get_fields(monoClass, &iter))) {
		if (!ShouldSerialize(field)) continue;
		const char* name = mono_field_get_name(field);
		if (!group.Has(name)) continue;
		auto& val = group.Get(name);

		std::visit([&](auto&& _arg) {
			using T = std::decay_t<decltype(_arg)>;
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, bool> || std::is_same_v<T, Vector2> || std::is_same_v<T, Vector3> || std::is_same_v<T, Vector4>) {
				mono_field_set_value(safeObj, field, (void*)&_arg);
			} else if constexpr (std::is_same_v<T, std::string>) {
				mono_field_set_value(safeObj, field, mono_string_new(mono_domain_get(), _arg.c_str()));
			} else if constexpr (std::is_same_v<T, std::shared_ptr<Variables::GenericObject>>) {
				MonoObject* obj = mono_field_get_value_object(mono_domain_get(), field, safeObj);
				if (obj) VarToMonoObject(obj, mono_object_get_class(obj), val);
			} else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Variables::GenericObject>>>) {
				MonoObject* list = mono_field_get_value_object(mono_domain_get(), field, safeObj);
				if (list) {
					MonoClass* lc = mono_object_get_class(list);
					MonoMethod* clear = mono_class_get_method_from_name(lc, "Clear", 0);
					mono_runtime_invoke(clear, list, nullptr, nullptr);
					MonoMethod* add = mono_class_get_method_from_name(lc, "Add", 1);
					MonoType* et = mono_signature_get_return_type(mono_method_signature(mono_class_get_method_from_name(lc, "get_Item", 1)));
					MonoClass* ek = mono_class_from_mono_type(et);
					for (auto& itemGen : _arg) {
						MonoObject* item = mono_object_new(mono_domain_get(), ek);
						mono_runtime_object_init(item);
						VarToMonoObject(item, ek, itemGen);
						void* args[1] = { item };
						mono_runtime_invoke(add, list, args, nullptr);
					}
				}
			}
			}, val);
	}
}

size_t Variables::AddGroup(const std::string& _name) {
	if (groupKeyMap_.contains(_name)) return groupKeyMap_.at(_name);
	Group group; group.name = _name;
	size_t index = groups_.size();
	groups_.push_back(group);
	groupKeyMap_[_name] = index;
	return index;
}

const Variables::Group& Variables::GetGroup(const std::string& _name) const { return groups_[groupKeyMap_.at(_name)]; }
bool Variables::HasGroup(const std::string& _name) const { return groupKeyMap_.contains(_name); }
const std::unordered_map<std::string, size_t>& Variables::GetGroupKeyMap() const { return groupKeyMap_; }
const std::vector<Variables::Group>& Variables::GetGroups() const { return groups_; }

void Variables::SetVariable(const std::string& _groupName, const std::string& _varName, const Var& _value) {
	size_t groupIdx = HasGroup(_groupName) ? groupKeyMap_.at(_groupName) : AddGroup(_groupName);
	groups_[groupIdx].Add(_varName, _value);
}

void ComponentDebug::VariablesDebug(Variables* _variables) {
	if (!_variables) return;
	if (ImGui::Button("export entity")) {
		GameEntity* entity = _variables->GetOwner();
		_variables->ReloadScriptVariables();
		nlohmann::json entityJson = EntityJsonConverter::ToJson(entity);
		std::string path = "Assets/Scene/" + entity->GetECSGroup()->GetGroupName() + "/" + entity->GetName() + ".entity";
		std::filesystem::create_directories(std::filesystem::path(path).parent_path());
		std::ofstream ofs(path);
		if (ofs) { ofs << entityJson.dump(4); Console::Log("Exported to: " + path); }
	}
}

const Variables::Var& Variables::Group::Get(const std::string& _name) const { return variables[keyMap.at(_name)]; }
bool Variables::Group::Has(const std::string& _name) const { return keyMap.contains(_name); }
