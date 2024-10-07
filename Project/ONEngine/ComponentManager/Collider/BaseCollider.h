#pragma once

#include <memory>

#include "ComponentManager/Base/BaseComponent.h"
#include "ComponentManager/Transform/Transform.h"

#include "GraphicManager/ModelManager/Model.h"

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

	Vec3 GetPosition() const;

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<Transform> transform_;
};