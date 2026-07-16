#pragma once

/// std
#include <string>
#include <unordered_map>
#include <memory>

/// external
#include <mono/jit/jit.h>
#include <imgui.h>


namespace ONEngine { class Variables; }

namespace Editor {

/// ///////////////////////////////////////////////////////
/// C#のフィールドをImGuiで表示するための名前空間
/// ///////////////////////////////////////////////////////
namespace CSGui {


/// @brief ImGuiでC#の[SerializeField]のフィールドを表示する
/// @param _scriptName スクリプト名
/// @param _type 変数の型
/// @param _obj 表示するオブジェクト
/// @param _field 表示するフィールド
/// @param _name ImGuiで表示する変数名
void ShowField(const std::string& _scriptName, int _type, MonoObject* _obj, MonoClassField* _field, const char* _name);

/// @brief ImGuiでVariablesコンポーネント経由でC#の[SerializeField]のフィールドを表示する
/// @param _vars Variablesコンポーネント
/// @param _groupName グループ名 (スクリプト名)
/// @param _type 変数の型
/// @param _field 表示するフィールド
/// @param _name ImGuiで表示する変数名
void ShowFieldForVariables(ONEngine::Variables* _vars, const std::string& _groupName, int _type, MonoClassField* _field, const char* _name);



/// @brief ImGuiでCSのフィールドを表示するための構造体
struct ImGuiShowField {
	virtual ~ImGuiShowField() = default;
	virtual void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) = 0;
};

/// @brief intをImGuiで表示するための構造体
struct IntField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief floatをImGuiで表示するための構造体
struct FloatField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief doubleをImGuiで表示するための構造体
struct DoubleField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief boolをImGuiで表示するための構造体
struct BoolField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief stringをImGuiで表示するための構造体
struct StringField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief ListをImGuiで表示するための構造体
struct ListField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief enumをImGuiで表示するための構造体
struct EnumField : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// -------------------------------------
/// 自作構造体の表示用
/// -------------------------------------

struct StructGui : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	void Register();
	std::unordered_map<std::string, std::unique_ptr<ImGuiShowField>> fieldDrawers; ///< フィールドの型ごとに表示用の構造体を保持
};

/// @brief Vector2をImGuiで表示するための構造体
struct Vector2Field : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief Vector3をImGuiで表示するための構造体
struct Vector3Field : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

/// @brief Vector4をImGuiで表示するための構造体
struct Vector4Field : public ImGuiShowField {
	void Draw(const std::string& _scriptName, MonoObject* _obj, MonoClassField* _field, const char* _name) override;
};

}

} /// Editor