#pragma once

/// engine
#include "GameObjectManager/GameObjectManager.h"

class PointLight : public BaseGameObject {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	struct BufferData final {
		Vec4 color;      /// 色
		Vec3 position;	 /// 座標
		float intensity; /// 輝度
		float radius;    /// ライトの届く最大距離
		float decay;     /// 減衰率
		int active;      /// 有効かどうか
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PointLight() { CreateTag(this); }
	~PointLight() {}

	void Initialize() override;
	void Update() override;
	void Debug() override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	BufferData data_;


public:

	/// ===================================================
	/// public : getter
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

	/// @brief ライトの届く最大距離を設定
	/// @param _radius 最大距離
	void SetRadius(float _radius);

	/// @brief ライトの減衰率を設定
	/// @param _decay 減衰率
	void SetDecay(float _decay);

	/// @brief ライトの有効無効を設定
	/// @param _active 有効無効
	void SetActive(bool _active);

};