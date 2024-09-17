#pragma once

#include <memory>

#include <Component/Base/BaseComponent.h>
#include <Component/Transform/Transform.h>
#include <Model.h>

/// ===================================================
/// 3Dコライダーの基底クラス
/// ===================================================
class BaseCollider : public BaseComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseCollider() {}
	~BaseCollider() {}

	void Initialize()	override {}
	void Update()		override {}
	void Draw()			override {}

	virtual void CreateCollider([[maybe_unused]] const Model* model) {}

	void UpdateMatrix();

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<Transform> transform_;
};