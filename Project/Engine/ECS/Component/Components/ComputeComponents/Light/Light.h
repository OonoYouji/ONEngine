#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ////////////////////////////////////////////////////////////
/// DirectionalLight
/// ////////////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DirectionalLight
 * @brief 無限遠からの平行光源（太陽光等）を表し、光源の向き、強度、およびライトカラーを保持するライトコンポーネントクラス
 */
class DirectionalLight : public IComponent {
	friend class AnimationPlayer;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	DirectionalLight();

	/**
	 * @brief デストラクタ
	 */
	~DirectionalLight() {}

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	float intensity_;
	Vector3 direction_;
	Vector4 color_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================
	
	/**
	 * @brief ライトの照度強度を設定します。
	 */
	void SetIntensity(float _intensity) { intensity_ = _intensity; }
	
	/**
	 * @brief ライトの照射方向（ベクトル）を設定します。
	 */
	void SetDirection(const Vector3& _direction) { direction_ = _direction; }
	
	/**
	 * @brief ライトのカラー（RGBA）を設定します。
	 */
	void SetColor(const Vector4& _color) { color_ = _color; }


	/**
	 * @brief 現在の照度強度を取得します。
	 */
	float GetIntensity() const { return intensity_; }
	
	/**
	 * @brief 現在の照射方向を取得します。
	 */
	const Vector3& GetDirection() const { return direction_; }

	/**
	 * @brief 現在のライトカラーを取得します。
	 */
	const Vector4& GetColor() const { return color_; }

	/**
	 * @brief アニメーション制御用：強度変数への参照を取得。
	 */
	float& GetIntensityForAnimation() { return intensity_; }

	/**
	 * @brief アニメーション制御用：カラー変数への参照を取得。
	 */
	Vector4& GetColorForAnimation() { return color_; }
};


} /// ONEngine
