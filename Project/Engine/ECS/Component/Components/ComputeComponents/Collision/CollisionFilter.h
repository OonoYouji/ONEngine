#pragma once

/// std
#include <cstdint>

namespace ONEngine {

///
/// 衝突レイヤーの定義
///
enum class CollisionFilter : uint32_t {
	Default      = 1 << 0,
	Player       = 1 << 1,
	PlayerBullet = 1 << 2,
	Enemy        = 1 << 3,
	EnemyBullet  = 1 << 4,
	StageObject  = 1 << 5,
	ALL = 0xFFFFFFFF
};

}