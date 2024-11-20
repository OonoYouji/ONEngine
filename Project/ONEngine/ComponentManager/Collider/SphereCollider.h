#pragma once

#include "ComponentManager/Collider/BaseCollider.h"
#include "GraphicManager/ModelManager/Model.h"


/// ===================================================
/// 球のコライダーのクラス
/// ===================================================
class SphereCollider final : public BaseCollider {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SphereCollider(Model* model) {
		CreateCollider(model);
	}
	~SphereCollider() {}

	void Initialize()	override;
	void Update()		override;
	void Draw()			override;

	/// <summary>
	/// コライダーの作成
	/// </summary>
	/// <param name="model">: sphereを生成するためのmodel</param>
	void CreateCollider(const Model* model) override;

	/// <summary>
	/// 半径のゲッタ
	/// </summary>
	/// <returns>半径</returns>
	float GetRadius() const {
		return radius_;
	}

	void SetRadius(float _radius) {
		radius_ = _radius;
	}

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	float	radius_ = 0.0f;
	Model*	sphere_ = nullptr;
};
