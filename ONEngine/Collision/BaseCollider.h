#pragma once

#include <Transform.h>
class BaseGameObject;
class Model;
class BoxCollider;

class BaseCollider {
public:

	virtual ~BaseCollider() = default;

	virtual void Initialize(BaseGameObject* gameObject, Model* model);

	/// <summary>
	/// 当たり判定の可視化
	/// </summary>
	virtual void Draw() {}

	virtual bool IsCollision([[maybe_unused]] BaseCollider* other) = 0;
	virtual bool IsCollision([[maybe_unused]] BoxCollider* box) = 0;

	void UpdateMatrix();

public:

	BaseGameObject* gameObject_ = nullptr;
	Transform transform_;

};