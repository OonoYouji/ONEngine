#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

namespace ONEngine {

namespace EntityJsonConverter {
/**
 * @brief ゲームエンティティの構成データ（トランスフォーム、アタッチされたコンポーネント、子エンティティなど）をJSONオブジェクトへシリアライズします。
 * @param _entity 対象エンティティ
 * @param _forceFull trueの場合、プレハブの差分ではなく完全なデータをシリアライズします
 * @return シリアライズされたJSONオブジェクト
 */
nlohmann::json ToJson(const GameEntity* _entity, bool _forceFull = false);


/**
 * @brief シリアライズされたJSONデータからゲームエンティティの構成要素（名前、有効状態、コンポーネントパラメータ等）をデシリアライズして復元・マージします。
 * @param _json ゲームエンティティのシリアライズJSONデータ
 * @param _entity 復元先のゲームエンティティポインタ
 * @param _groupName 所属するECSGroup名
 * @param _merge trueの場合は既存の構成にマージし、falseの場合は上書きします
 */
void FromJson(const nlohmann::json& _json, GameEntity* _entity, const std::string& _groupName, bool _merge = true);

/**
 * @brief ゲームエンティティのTransformコンポーネントに限定してJSONデータからトランスフォームパラメータ（位置、回転、縮尺等）を復元適用します。
 * @param _json トランスフォームデータを含むJSONオブジェクト
 * @param _entity 反映先のゲームエンティティポインタ
 */
void TransformFromJson(const nlohmann::json& _json, GameEntity* _entity);
};

} /// namespace ONEngine