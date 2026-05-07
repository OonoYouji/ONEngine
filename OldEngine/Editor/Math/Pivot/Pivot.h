#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Math/Vector2.h"


namespace ONEngine {
class AssetCollection;
class EntityComponentSystem;
}


namespace Editor {

void UpdatePivot(ONEngine::EntityComponentSystem* _ecs);

void SetEntity(const ONEngine::Guid& _guid);
void ClearEntity();

void SetDrawRect(const ONEngine::Vector2& _pos, const ONEngine::Vector2& _size);


///// @brief 現在選択されているもののGuidリストを取得する (１つしか選択されていない場合もベクターで返す)
///// @return 選択されているエンティティのGuidリスト
//const std::vector<ONEngine::Guid>& GetSelectedEntityGuids();
//
///// @brief 操作したいエンティティのGuidを追加する
///// @param _guid エンティティのGuid
//void AddSelectedEntityGuid(const ONEngine::Guid& _guid);
//
///// @brief 操作したいエンティティのGuidリストを設定する
///// @param _guids 操作対象のエンティティのGuidリスト
//void SetSelectedEntityGuids(const std::vector<ONEngine::Guid>& _guids);
//
///// @brief 選択しているエンティティを解除
//void ClearSelectedEntityGuids();

} /// namespace Editor