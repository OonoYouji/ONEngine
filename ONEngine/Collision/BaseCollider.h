#pragma once

#include <Transform.h>

#define MAYBE_UNUSED [[maybe_unused]]

class BaseGameObject;
class Model;

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
	virtual bool IsCollision(MAYBE_UNUSED class BoxCollider* box) { return false; };
	virtual bool IsCollision(MAYBE_UNUSED class AABBCollider* aabb) { return false; };
	virtual bool IsCollision(MAYBE_UNUSED class OBBCollider* obb) { return false; };
	virtual bool IsCollision(MAYBE_UNUSED class SphereCollider* sphere) { return false; };

	void UpdateMatrix();

public:

	BaseGameObject* gameObject_ = nullptr;
	Transform transform_;

};