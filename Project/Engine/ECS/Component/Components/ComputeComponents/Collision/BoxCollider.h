#pragma once

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "ICollider.h"
#include "Engine/Core/Utility/Math/Vector3.h"


namespace ONEngine {

class BoxCollider;

namespace ComponentDebug {
    /**
     * @brief エディタ用：BoxColliderコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
    void BoxColliderDebug(BoxCollider* _boxCollider);

}	/// namespace ComponentDebug

/**
 * @class BoxCollider
 * @brief 直方体（AABB）の形状を持つコライダーコンポーネントクラス
 */
class BoxCollider : public ICollider {
	/// --------------- friend function --------------- ///
	friend void ComponentDebug::BoxColliderDebug(BoxCollider* _boxCollider);
	friend void from_json(const nlohmann::json& _j, BoxCollider& _b);
	friend void to_json(nlohmann::json& _j, const BoxCollider& _b);
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	/**
	 * @brief コンストラクタ
	 */
	BoxCollider();

	/**
	 * @brief デストラクタ
	 */
	~BoxCollider() override = default;

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	Vector3 size_;

public:
	/// =====================================================
	/// public : accessors
	/// =====================================================

	/**
	 * @brief 直方体コライダーのサイズを設定します。
	 */
	void SetSize(const Vector3& _size);

	/**
	 * @brief 直方体コライダーのサイズを取得します。
	 */
	const Vector3& GetSize() const;

};

/// @brief Mono Internal Calls

/**
 * @brief C#（Mono）インターフェース用：コライダーサイズを取得
 */
Vector3 InternalGetSize(uint64_t _nativeHandle);

/**
 * @brief C#（Mono）インターフェース用：コライダーサイズを設定
 */
void InternalSetSize(uint64_t _nativeHandle, Vector3 _size);

/**
 * @brief C#（Mono）インターフェース用：トリガーモードかの判定を取得
 */
bool InternalIsTriggerBox(uint64_t _nativeHandle);

/**
 * @brief C#（Mono）インターフェース用：トリガーモードの設定を変更
 */
void InternalSetTriggerBox(uint64_t _nativeHandle, bool _trigger);

/**
 * @brief C#（Mono）インターフェース用：質量を取得
 */
float InternalGetMassBox(uint64_t _nativeHandle);

/**
 * @brief C#（Mono）インターフェース用：質量を設定
 */
void InternalSetMassBox(uint64_t _nativeHandle, float _mass);

} /// ONEngine
