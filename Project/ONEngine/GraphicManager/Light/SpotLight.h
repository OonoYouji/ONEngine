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

	const BufferData& GetData() const { return data_; }

	/// @brief 色のセット
	void SetColor(const Vec4& _color);

	void SetIntensity(float _intensity);

	void SetDirection(const Vec3& _direction);

	void SetDecay(float _decay);

	void SetCosAngle(float _cosAngle);

	void SetCosFalloffStart(float _cosFalloffStart);

	void SetActive(bool _active);

};
