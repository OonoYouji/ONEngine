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


namespace ImGuiSelection {

	/// @brief 選択しているオブジェクトのGuidを返す
	/// @return オブジェクトのGuid
	const Guid& GetSelectedObject();

	/// @brief 選択したオブジェクトのGuidを設定する
	/// @param _entityGuid オブジェクトのGuid
	void SetSelectedEntity(const Guid& _entityGuid, SelectionType _type);

	/// @brief 選択しているオブジェクトの種類を返す
	/// @return オブジェクトの種類
	SelectionType GetSelectionType();

};