#pragma once

#include "BaseCollider.h"
#include <Model.h>
#include <Vector3.h>
#include <Transform.h>

class BoxCollider : public BaseCollider {
public:

	BoxCollider() {}
	~BoxCollider() {}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="gameObejct"></param>
	/// <param name="model"></param>
	void Initialize(BaseGameObject* gameObejct, Model* model) override;


	/// <summary>
	/// HitBoxの描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// Box to Box Collision
	/// </summary>
	/// <param name="box"></param>
	/// <returns></returns>
	bool IsCollision(BaseCollider* other) override;
	bool IsCollision(BoxCollider* box) override;

private:

	std::vector<Vec3> GetVertices() const;

	void UpdateOrientatinos();

private:

	Vec3 size_;
	Vec3 orientatinos_[3];

	Model* cube_ = nullptr;

};