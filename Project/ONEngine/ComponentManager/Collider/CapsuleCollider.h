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

	void SetTransformArray(const std::array<Transform*, 2>& _transformArray);

private:

	Model* sphere_ = nullptr;
	Model* tube_   = nullptr;

	std::array<Transform*, 2> transformArray_;

	Vec3 direction_{};
	float  lenght_;

	Vec3 upDirection_ = Vec3::kUp;
};