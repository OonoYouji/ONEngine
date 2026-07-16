#pragma once

/// engine
#include "ICollider.h"


namespace ONEngine {

class SphereCollider;

namespace ComponentDebug {
    /**
     * @brief エディタ用：SphereColliderコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
    void SphereColliderDebug(SphereCollider* _collider);
}

/**
 * @class SphereCollider
 * @brief 球体の形状を持つコライダーコンポーネントクラス
 */
class SphereCollider : public ICollider {
	friend void ComponentDebug::SphereColliderDebug(SphereCollider* _collider);
	friend void from_json(const nlohmann::json& _j, SphereCollider& _c);
	friend void to_json(nlohmann::json& _j, const SphereCollider& _c);
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	/**
	 * @brief コンストラクタ
	 */
	SphereCollider();

	/**
	 * @brief デストラクタ
	 */
	~SphereCollider() override = default;

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	float radius_;

public:
	/// =====================================================
	/// public : accessors
	/// =====================================================

	/**
	 * @brief 球体コライダーの半径を設定します。
	 */
	void SetRadius(float _radius);

	/**
	 * @brief 球体コライダーの半径を取得します。
	 */
	float GetRadius() const;

};

/// @brief Mono Internal Calls

/**
 * @brief C#（Mono）インターフェース用：コライダーの半径を取得
 */
float InternalGetRadius(uint64_t _nativeHandle);

/**
 * @brief C#（Mono）インターフェース用：コライダーの半径を設定
 */
void InternalSetRadius(uint64_t _nativeHandle, float _radius);

/**
 * @brief C#（Mono）インターフェース用：トリガーモードかの判定を取得
 */
bool InternalIsTriggerSphere(uint64_t _nativeHandle);

/**
 * @brief C#（Mono）インターフェース用：トリガーモードの設定を変更
 */
void InternalSetTriggerSphere(uint64_t _nativeHandle, bool _trigger);

/**
 * @brief C#（Mono）インターフェース用：質量を取得
 */
float InternalGetMass(uint64_t _nativeHandle);

/**
 * @brief C#（Mono）インターフェース用：質量を設定
 */
void InternalSetMass(uint64_t _nativeHandle, float _mass);

} /// ONEngine
