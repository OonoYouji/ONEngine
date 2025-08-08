#include "ImGuiShowField.h"

/// external
#include <mono/metadata/object.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "ImGuiMath.h"


namespace {
	std::unordered_map<int, std::unique_ptr<CSGui::ImGuiShowField>> gFieldDrawers;

	void RegisterFieldDrawers() {
		/// ここで必要なフィールドドロワーを登録する
		gFieldDrawers[MONO_TYPE_I4] = std::make_unique<CSGui::IntField>();
		gFieldDrawers[MONO_TYPE_R4] = std::make_unique<CSGui::FloatField>();
		gFieldDrawers[MONO_TYPE_R8] = std::make_unique<CSGui::DoubleField>();
		gFieldDrawers[MONO_TYPE_BOOLEAN] = std::make_unique<CSGui::BoolField>();
		gFieldDrawers[MONO_TYPE_STRING] = std::make_unique<CSGui::StringField>();
		gFieldDrawers[MONO_TYPE_SZARRAY] = std::make_unique<CSGui::ArrayField>();
		gFieldDrawers[MONO_TYPE_ARRAY] = std::make_unique<CSGui::ArrayField>();
		gFieldDrawers[MONO_TYPE_VALUETYPE] = std::make_unique<CSGui::StructGui>();

		// 他の型も同様に登録
		Console::Log("Field drawers registered.");
	}

}


void CSGui::ShowFiled(int _type, MonoObject* _obj, MonoClassField* _field, const char* _name) {
	if (gFieldDrawers.empty()) {
		RegisterFieldDrawers();  ///< 初回呼び出し時にフィールドドロワーを登録
	}

	if (gFieldDrawers.find(_type) == gFieldDrawers.end()) {
		Console::Log("[error] Unsupported field type: " + std::to_string(_type));
		return;
	}

	gFieldDrawers[_type]->Draw(_obj, _field, _name);
}



void CSGui::IntField::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	int value = 0;
	mono_field_get_value(_obj, _field, &value);
	if (ImGui::DragInt(_name, &value)) {
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &value);
	}
}

void CSGui::FloatField::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	float value = 0.0f;
	mono_field_get_value(_obj, _field, &value);
	if (ImGui::DragFloat(_name, &value)) {
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &value);
	}
}


void CSGui::DoubleField::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	double value = 0.0;
	mono_field_get_value(_obj, _field, &value);

	/// ImGuiはfloatしかサポートしていないので、floatにキャストして表示
	float floatValue = static_cast<float>(value);
	if (ImGui::DragFloat(_name, &floatValue)) {
		value = static_cast<double>(floatValue);
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &value);
	}
}


void CSGui::BoolField::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	bool value = false;
	mono_field_get_value(_obj, _field, &value);
	if (ImGui::Checkbox(_name, &value)) {
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &value);
	}
}

void CSGui::StringField::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	// ① Mono string を取得
	MonoString* monoStr = (MonoString*)mono_field_get_value_object(mono_domain_get(), _field, _obj);
	if (!monoStr) return;

	// ② UTF-8 へ変換（リークしないよう dup する）
	char* utf8 = mono_string_to_utf8(monoStr);
	std::string value = utf8;
	mono_free(utf8); // ← 解放を忘れずに！

	// ③ ImGui で編集
	if (ImGuiInputText(_name, &value, ImGuiInputTextFlags_EnterReturnsTrue)) {
		// ④ 変更されたら MonoString を生成し、再セット
		MonoString* newStr = mono_string_new(mono_domain_get(), value.c_str());
		mono_field_set_value(_obj, _field, newStr);
	}
}


void CSGui::ArrayField::Draw(MonoObject*, MonoClassField*, const char*) {
	/// TODO: 配列の表示、編集 
	/// 多次元配列の対応は今後考える
}

void CSGui::StructGui::Draw(MonoObject* _obj, MonoClassField* _field, [[maybe_unused]] const char* _name) {
	if (fieldDrawers.empty()) {
		Register();  ///< 初回呼び出し時にフィールドドロワーを登録
	}

	MonoType* fieldType = mono_field_get_type(_field);
	MonoClass* fieldClass = mono_class_from_mono_type(fieldType);
	const char* name = mono_class_get_name(fieldClass);

	auto itr = fieldDrawers.find(name);
	if (itr == fieldDrawers.end()) {
		Console::Log("[error] Unsupported struct type: " + std::string(name));
		return;
	}

	/// フィールドドロワーが登録されている場合はそれを使用
	itr->second->Draw(_obj, _field, _name);
}

void CSGui::StructGui::Register() {
	/// フィールドドロワーを登録する
	fieldDrawers["Vector2"] = std::make_unique<Vector2Field>();
	fieldDrawers["Vector3"] = std::make_unique<Vector3Field>();
	fieldDrawers["Vector4"] = std::make_unique<Vector4Field>();
}


void CSGui::Vector2Field::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	Vector2 structData;
	mono_field_get_value(_obj, _field, &structData);

	if (ImGui::DragFloat2(_name, &structData.x)) {
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &structData);
	}
}

void CSGui::Vector3Field::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	Vector3 structData;
	mono_field_get_value(_obj, _field, &structData);

	if (ImGui::DragFloat3(_name, &structData.x)) {
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &structData);
	}
}

void CSGui::Vector4Field::Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) {
	Vector4 structData;
	mono_field_get_value(_obj, _field, &structData);

	if (ImGui::DragFloat4(_name, &structData.x)) {
		/// 編集した値をセットする
		mono_field_set_value(_obj, _field, &structData);
	}
}
