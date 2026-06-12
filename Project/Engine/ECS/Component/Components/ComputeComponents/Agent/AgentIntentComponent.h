#pragma once

/// std
#include <nlohmann/json_fwd.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include <Engine/Core/Utility/Math/Vector3.h>
#include <Engine/Core/Utility/Math/Quaternion.h>


/// ----- 前方宣言 ----- ///
namespace ONEngine {

class AgentIntentComponent;

/// Json変換
void from_json(const nlohmann::json& _j, AgentIntentComponent& _c);
void to_json(nlohmann::json& _j, const AgentIntentComponent& _c);

namespace ComponentDebug {
void AgentIntentComponentDebug(AgentIntentComponent* comp);
}


/// ///////////////////////////////////////////////////
/// AIの「意図」を格納するコンポーネント
/// C#側で計算され、C++側で行動に変換される
/// ///////////////////////////////////////////////////
class AgentIntentComponent : public IComponent {
public:
	/// <summary>
	/// C++とC#でデータを一括同期するための構造体
	/// </summary>
	struct BatchData {
	    uint32_t compId;
	    Vector3 desiredMoveDirection;
	    Quaternion desiredRotation;
	    float rotationSpeed;
	    float maxSpeed;
	    uint8_t useDesiredRotation; // bool interop
	    uint8_t isAttacking; // boolの代わりにuint8_tを使用して互換性を確保
	    int32_t targetEntityId;
	};
	/// ----- friend class ----- ///
	friend class MovementSystem;

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AgentIntentComponent() {
		Reset();
	}

	void Reset() override {
		desiredMoveDirection = Vector3::Zero;
		desiredRotation = Quaternion::kIdentity;
		rotationSpeed = 5.0f; // Default rotation speed
		maxSpeed = 8.0f;      // Default movement speed
		useDesiredRotation = false;
		isAttacking = false;
		targetEntityId = 0; // 0: invalid id
	}

public:
	/// ===================================================
	/// public : objects
	/// ===================================================

	/// @brief C#側が設定する「移動したい方向」
	Vector3 desiredMoveDirection;

	/// @brief C#側が設定する「回転したい向き」
	Quaternion desiredRotation;

	/// @brief 旋回速度 (rad/s 相当の Lerp 係数)
	float rotationSpeed;

	/// @brief 最大移動速度 (C#側から設定可能)
	float maxSpeed;

	/// @brief 現在の移動速度（加減速計算用、C++内部保持）
	float currentSpeed = 0.0f;

	/// @brief 回転制御を有効にするか
	bool useDesiredRotation;

	/// @brief C#側が設定する「攻撃中か」のフラグ
	bool isAttacking;

	/// @brief C#側が設定する「追従対象のEntityID」
	int32_t targetEntityId;
};

} /// ONEngine
