#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ////////////////////////////////////////////////////////////
/// DirectionalLight
/// ////////////////////////////////////////////////////////////
namespace ONEngine {

class DirectionalLight : public IComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DirectionalLight();
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
	
	/// @brief 光の強度の設定
	/// @param _intensity 光の強度
	void SetIntensity(float _intensity) { intensity_ = _intensity; }
	
	/// @brief 光の方向の設定
	/// @param _direction 光の方向
	void SetDirection(const Vector3& _direction) { direction_ = _direction; }
	
	/// @brief 光の色の設定
	/// @param _color 光の色
	void SetColor(const Vector4& _color) { color_ = _color; }


	/// @brief 光の強度の取得
	/// @return 光の強度
	float GetIntensity() const { return intensity_; }
	
	/// @brief 光の方向の取得
	/// @return 光の方向
	const Vector3& GetDirection() const { return direction_; }

	/// @brief 光の色の取得
	/// @return 光の色
	const Vector4& GetColor() const { return color_; }
};


} /// ONEngine
