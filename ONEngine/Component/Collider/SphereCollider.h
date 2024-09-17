#pragma once

#include <Component/Collider/BaseCollider.h>

#include <Model.h>


/// ===================================================
/// 球のコライダーのクラス
/// ===================================================
class SphereCollider final : public BaseCollider {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SphereCollider() {}
	~SphereCollider() {}

	void Initialize()	override;
	void Update()		override;
	void Draw()			override;

	/// <summary>
	/// コライダーの作成
	/// </summary>
	/// <param name="model">: sphereを生成するためのmodel</param>
	void CreateCollider(const Model* model) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	float	radius_ = 0.0f;
	Model*	sphere_ = nullptr;
};
