#pragma once

#include <array>

#include "BaseCollider.h"
#include <Model.h>
#include <Vector3.h>
#include <Transform.h>

class BoxCollider : public BaseCollider {
public:

	BoxCollider() {}
	~BoxCollider() {}


	void Initialize(BaseGameObject* gameObejct, Model* model) override;
	void Update() override;
	void Draw() override;

	std::vector<Vec3> GetVertices() const;
	const std::array<Vec3, 3>& GetOrientations() const {
		return orientatinos_;
	}

	const Vec3& GetSize() const {
		return size_;
	}

	const Mat4& GetObbMatTransform() const {
		return obbMatTransform_;
	}

private:
	void UpdateOrientatinos();
private:

	Vec3 size_;
	std::array<Vec3, 3> orientatinos_;

	Model* cube_ = nullptr;
	Mat4 obbMatTransform_;

};