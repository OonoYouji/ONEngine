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

Vector3 EffectEmitShape::GetEmitDirection(const Vector3& _emitedPosition) {
	Vector3 direction = Vector3::kZero;
	/// 形状ごとに発生方向を取得する
	switch (shapeType_) {
	case ShapeType::Sphere:
		direction = _emitedPosition - sphere_.center;
		break;
	case ShapeType::Cube:
		direction = _emitedPosition - cube_.center;
		break;
	case ShapeType::Cone:
		direction = _emitedPosition - cone_.center;
		break;
	};

	return direction.Normalize();
}

void EffectEmitShape::SetShapeType(ShapeType _type) {
	shapeType_ = _type;
}

void EffectEmitShape::SetSphere(const Vector3& _center, float _radius) {
	shapeType_ = ShapeType::Sphere;
	sphere_.center = _center;
	sphere_.radius = _radius;
}

void EffectEmitShape::SetSphere(const Sphere& _sphere) {
	shapeType_ = ShapeType::Sphere;
	sphere_ = _sphere;
}

void EffectEmitShape::SetCube(const Vector3& _center, const Vector3& _size) {
	shapeType_ = ShapeType::Cube;
	cube_.center = _center;
	cube_.size = _size;
}

void EffectEmitShape::SetCube(const Cube& _cube) {
	shapeType_ = ShapeType::Cube;
	cube_ = _cube;
}

void EffectEmitShape::SetCone(const Vector3& _center, float _angle, float _radius, float _height) {
	shapeType_ = ShapeType::Cone;
	cone_.center = _center;
	cone_.angle = _angle;
	cone_.radius = _radius;
	cone_.height = _height;
}

void EffectEmitShape::SetCone(const Cone& _cone) {
	shapeType_ = ShapeType::Cone;
	cone_ = _cone;
}

Vector3 EffectEmitShape::GetCenter() const {
	switch (shapeType_) {
	case ShapeType::Cube: return cube_.center;
	case ShapeType::Cone: return cone_.center;
	default: return Vector3::kZero;
	}
}

EffectEmitShape::ShapeType EffectEmitShape::GetType() const {
	return shapeType_;
}

EffectEmitShape::Sphere EffectEmitShape::GetSphere() const {
	return sphere_;
}

EffectEmitShape::Cube EffectEmitShape::GetCube() const {
	return cube_;
}

EffectEmitShape::Cone EffectEmitShape::GetCone() const {
	return cone_;
}

