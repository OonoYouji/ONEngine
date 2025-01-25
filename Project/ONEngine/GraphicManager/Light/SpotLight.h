#pragma once

#include "GameObjectManager/BaseGameObject.h"

class SpotLight : public BaseGameObject {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	struct BufferData final {
		Vec4  color;
		Vec3  position;
		float intensity;
		Vec3  direction;
		float distance;
		float decay;
		float cosAngle;
		float cosFalloffStart;
		int   active;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SpotLight();
	~SpotLight();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	BufferData data_;


public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// @brief buffer dataの取得
	/// @return buffer data
	const BufferData& GetData() const { return data_; }

	/// @brief ライトの色を設定
	/// @param _color 色
	void SetColor(const Vec4& _color);

	/// @brief ライトの輝度を設定
	/// @param _intensity 輝度
	void SetIntensity(float _intensity);

	/// @brief ライトの方向を設定
	/// @param _direction 方向
	void SetDirection(const Vec3& _direction);

	/// @brief ライトの減衰率を設定
	/// @param _decay 減衰率
	void SetDecay(float _decay);

	/// @brief ライトの角度を設定
	/// @param _cosAngle 角度
	void SetCosAngle(float _cosAngle);

	/// @brief ライトの減衰開始角度を設定
	/// @param _cosFalloffStart 減衰開始角度
	void SetCosFalloffStart(float _cosFalloffStart);

	/// @brief ライトの有効無効を設定
	/// @param _active 有効無効
	void SetActive(bool _active);

};
