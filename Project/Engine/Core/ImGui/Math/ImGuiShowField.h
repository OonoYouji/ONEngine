#pragma once

/// std
#include <string>
#include <unordered_map>
#include <memory>

/// external
#include <mono/jit/jit.h>
#include <imgui.h>


/// ///////////////////////////////////////////////////////
/// C#のフィールドをImGuiで表示するための名前空間
/// ///////////////////////////////////////////////////////
namespace CSGui {


	/// @brief ImGuiでC#の[SerializeField]のフィールドを表示する
	/// @param _type 変数の型
	/// @param _obj 表示するオブジェクト
	/// @param _field 表示するフィールド
	/// @param _name ImGuiで表示する変数名
	void ShowFiled(int _type, MonoObject* _obj, MonoClassField* _field, const char* _name);



	/// @brief ImGuiでCSのフィールドを表示するための構造体
	struct ImGuiShowField {
		virtual ~ImGuiShowField() = default;
		virtual void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) = 0;
	};

	/// @brief intをImGuiで表示するための構造体
	struct IntField : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};

	/// @brief floatをImGuiで表示するための構造体
	struct FloatField : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};

	/// @brief doubleをImGuiで表示するための構造体
	struct DoubleField : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};

	/// @brief boolをImGuiで表示するための構造体
	struct BoolField : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};

	/// @brief stringをImGuiで表示するための構造体
	struct StringField : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};


	/// -------------------------------------
	/// 自作構造体の表示用
	/// -------------------------------------

	struct StructGui : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
		void Register();
		std::unordered_map<std::string, std::unique_ptr<ImGuiShowField>> fieldDrawers; ///< フィールドの型ごとに表示用の構造体を保持
	};

	/// @brief Vector2をImGuiで表示するための構造体
	struct Vector2Field : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};
	
	/// @brief Vector3をImGuiで表示するための構造体
	struct Vector3Field : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};
	
	/// @brief Vector4をImGuiで表示するための構造体
	struct Vector4Field : public ImGuiShowField {
		void Draw(MonoObject* _obj, MonoClassField* _field, const char* _name) override;
	};

}
