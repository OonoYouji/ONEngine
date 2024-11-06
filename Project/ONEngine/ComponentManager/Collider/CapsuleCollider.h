#pragma once

/// std
#include <array>

/// base class
#include "BaseCollider.h"


class CapsuleCollider final : public BaseCollider {
public:

	CapsuleCollider();
	~CapsuleCollider();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

	void CreateCollider([[maybe_unused]] const Model* model) override;

	/// <summary>
	/// カプセルの端をセット
	/// </summary>
	/// <param name="_positionArray"></param>
	void SetPositionArray(const std::array<Vec3*, 2>& _positionArray);

	Vec3 GetStartPosition() const;
	Vec3 GetEndPosition()   const;

	const Vec3& GetDirection() const { return direction_; }
	float GetLenght() const { return lenght_; }

	float GetRadius() const { return radius_; }
	void SetRadius(float _radius);

private:

	Model* sphere_ = nullptr;
	Model* tube_   = nullptr;

	std::array<Vec3*, 2>     positionArray_;
	std::array<Transform, 2> transformArray_;

	Vec3  direction_{};
	float lenght_;

	float radius_ = 1.0f;
};