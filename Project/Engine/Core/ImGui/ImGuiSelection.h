#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"

/// @brief 選択しているオブジェクトの種類
enum class SelectionType {
	None,     /// 何も選択されていない
	Entity,   /// シーン内のエンティティ
	Asset,    /// Projectビュー内のアセット
	Script,   /// スクリプト
};


/// @brief Gui上で選択しているオブジェクトに関する関数群
namespace ImGuiSelection {

	/// @brief 選択しているオブジェクトのGuidを返す
	/// @return オブジェクトのGuid
	const Guid& GetSelectedObject();

	/// @brief 選択したオブジェクトのGuidを設定する
	/// @param _entityGuid オブジェクトのGuid
	void SetSelectedObject(const Guid& _entityGuid, SelectionType _type);

	/// @brief 選択しているオブジェクトの種類を返す
	/// @return オブジェクトの種類
	SelectionType GetSelectionType();

};


/// @brief Guiの情報
namespace ImGuiInfo {

	/// @brief ImGuiの情報文字列を取得する
	/// @return string型の情報文字列
	const std::string& GetInfo();

	/// @brief ImGuiの情報文字列を設定する
	/// @param _info 設定する情報文字列
	void SetInfo(const std::string& _info);

}