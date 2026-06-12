#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include <vector>
#include <unordered_set>

namespace Editor {

/// @brief 選択しているオブジェクトの種類
enum class SelectionType {
	None,     /// 何も選択されていない
	Entity,   /// シーン内のエンティティ
	Asset,    /// Projectビュー内のアセット
	Script,   /// スクリプト
	Count     /// SelectionTypeのカウント
};


/// @brief Gui上で選択しているオブジェクトに関する関数群
namespace ImGuiSelection {

/// @brief 選択しているオブジェクトのGuidを返す (複数選択対応)
/// @return オブジェクトのGuidのリスト
const std::unordered_set<ONEngine::Guid>& GetSelectedObjects();

/// @brief 最後に選択したオブジェクトのGuidを返す
const ONEngine::Guid& GetLastSelectedObject();

/// @brief 選択したオブジェクトを単一設定する (既存の選択はクリア)
/// @param _guid オブジェクトのGuid
void SetSelectedObject(const ONEngine::Guid& _guid, SelectionType _type);

/// @brief 選択を追加する
void AddSelectedObject(const ONEngine::Guid& _guid, SelectionType _type);

/// @brief 選択を解除する
void RemoveSelectedObject(const ONEngine::Guid& _guid);

/// @brief 選択をすべてクリアする
void ClearSelection();

/// @brief 指定したGuidが選択されているか
bool IsSelected(const ONEngine::Guid& _guid);

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

} /// namespace Editor

