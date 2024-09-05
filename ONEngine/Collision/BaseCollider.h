#pragma once

#include <Transform.h>

#define MAYBE_UNUSED [[maybe_unused]]

class BaseGameObject;
class Model;
class BoxCollider;
class SphereCollider;


class BaseCollider {
public:

	virtual ~BaseCollider() = default;

	virtual void Initialize(BaseGameObject* gameObject, Model* model);

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() {}

	/// <summary>
	/// 当たり判定の可視化
	/// </summary>
	virtual void Draw() {}

	bool IsCollision(MAYBE_UNUSED BaseCollider* other);

	void UpdateMatrix();

	Vec3 GetPosition() const;

public:

	BaseGameObject* gameObject_ = nullptr;
	Transform transform_;

};

namespace Collision {

	bool BoxToBox(BoxCollider* a, BoxCollider* b);
	bool BoxToSphere(BoxCollider* box, SphereCollider* sphere);
	bool SphereToSphere(SphereCollider* a, SphereCollider* b);

}