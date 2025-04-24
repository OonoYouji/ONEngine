#include "EffectEmitShape.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/Utility/Utility.h"

EffectEmitShape::EffectEmitShape() {
	/// 球体の初期化
	sphere_.center = Vector3::kZero;
	sphere_.radius = 1.0f;
	/// 立方体の初期化
	cube_.center = Vector3::kZero;
	cube_.size = Vector3(1.0f, 1.0f, 1.0f);
	/// 円柱の初期化
	//cylinder_.center = Vector3::kZero;
	/*cylinder_.radius = 1.0f;
	cylinder_.height = 1.0f;*/
	/// 円錐の初期化
	cone_.center = Vector3::kZero;
	cone_.angle = 30.0f;
	cone_.radius = 1.0f;
	cone_.height = 1.0f;
}

EffectEmitShape::EffectEmitShape(const EffectEmitShape& _shape) {
	shapeType_ = _shape.shapeType_;
	switch (shapeType_) {
	case ShapeType::Sphere:   sphere_ = _shape.sphere_;     break;
	case ShapeType::Cube:     cube_ = _shape.cube_;         break;
	//case ShapeType::Cylinder: cylinder_ = _shape.cylinder_; break;
	case ShapeType::Cone:     cone_ = _shape.cone_;         break;
	}
}


Vector3 EffectEmitShape::GetEmitPosition() {
	/// 形状ごとに発生位置を取得する
	switch (shapeType_) {
	case ShapeType::Sphere:
	{
		float theta = Random::Float(0.0f, 2.0f * std::numbers::pi_v<float>);
		float phi = Random::Float(0.0f, std::numbers::pi_v<float>);
		float r = Random::Float(0.0f, sphere_.radius);
		return sphere_.center + Vector3(
			r * std::sin(phi) * std::cos(theta),
			r * std::cos(phi),
			r * std::sin(phi) * std::sin(theta)
		);
	}

	case ShapeType::Cube:
	{
		return cube_.center + Vector3(
			Random::Float(-cube_.size.x, cube_.size.x),
			Random::Float(-cube_.size.y, cube_.size.y),
			Random::Float(-cube_.size.z, cube_.size.z)
		);
	}

	//case ShapeType::Cylinder:
	//{
	//	float theta = Random::Float(0.0f, 2.0f * std::numbers::pi_v<float>);
	//	float r = Random::Float(0.0f, cylinder_.radius);
	//	return cylinder_.center + Vector3(
	//		r * std::cos(theta),
	//		Random::Float(0.0f, cylinder_.height),
	//		r * std::sin(theta)
	//	);
	//}

	case ShapeType::Cone:
	{
		float theta = Random::Float(0.0f, 2.0f * std::numbers::pi_v<float>);
		float r = Random::Float(0.0f, cone_.radius);
		return cone_.center + Vector3(
			r * std::cos(theta),
			Random::Float(0.0f, cone_.height),
			r * std::sin(theta)
		);
	}

	default:
		break;
	}

	return Vector3();
}
