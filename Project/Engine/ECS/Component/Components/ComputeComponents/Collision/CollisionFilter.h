#pragma once

/// std
#include <cstdint>

namespace ONEngine {

///
/// 衝突レイヤーの定義
///
/**
 * @enum CollisionFilter
 * @brief 衝突判定処理でオブジェクトを識別・フィルタリングするためのビットマスク用衝突レイヤー定義列挙型
 */
enum class CollisionFilter : uint32_t {
	Default      = 1 << 0, ///< デフォルト（未設定）
	Player       = 1 << 1, ///< プレイヤーキャラクター
	PlayerBullet = 1 << 2, ///< プレイヤーの攻撃弾
	Enemy        = 1 << 3, ///< 敵キャラクター
	EnemyBullet  = 1 << 4, ///< 敵の攻撃弾
	StageObject  = 1 << 5, ///< ステージ設置物・地形オブジェクト等
	ALL = 0xFFFFFFFF        ///< 全てのレイヤー
};

}