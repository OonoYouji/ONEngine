#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"

/// /////////////////////////////////////////////////////////////////
/// エフェクトの発生形状を指定するクラス
/// /////////////////////////////////////////////////////////////////
class EffectEmitShape final {
public:
	/// =================================================
	/// public : sub class
	/// =================================================

	enum class ShapeType {
		Sphere,
		Cube,
		Cone,
	};

	struct Sphere {
		Vector3 center;
		float radius;
	};

	struct Cube {
		Vector3 center;
		Vector3 size;
	};

	struct Cone {
		Vector3 center;
		float angle;  // 円錐の角度
		float radius; // 円の半径
		float height; // 円錐の高さ
	};


public:
	/// =================================================
	/// public : methods
	/// =================================================

	EffectEmitShape();
	EffectEmitShape(const EffectEmitShape& _shape);
	~EffectEmitShape() = default;

	EffectEmitShape& operator= (const EffectEmitShape& _shape);

	Vector3 GetEmitPosition();

private:
	/// =================================================
	/// private : objects
	/// =================================================
	ShapeType shapeType_ = ShapeType::Cone;
	union {
		Sphere sphere_;
		Cube cube_;
		Cone cone_;
	};


public:
	///	===========================================
	/// public : accessors
	///	===========================================

	void SetShapeType(ShapeType _type) { shapeType_ = _type; }

	void SetSphere(const Vector3& _center, float _radius) {
		shapeType_ = ShapeType::Sphere;
		sphere_.center = _center;
		sphere_.radius = _radius;
	}

	void SetCube(const Vector3& _center, const Vector3& _size) {
		shapeType_ = ShapeType::Cube;
		cube_.center = _center;
		cube_.size = _size;
	}

	void SetCone(const Vector3& _center, float _angle, float _radius, float _height) {
		shapeType_ = ShapeType::Cone;
		cone_.center = _center;
		cone_.angle = _angle;
		cone_.radius = _radius;
		cone_.height = _height;
	}

	

	Vector3 GetCenter() const {
		switch (shapeType_) {
		case ShapeType::Sphere:   return sphere_.center;
		case ShapeType::Cube:     return cube_.center;
		//case ShapeType::Cylinder: return cylinder_.center;
		case ShapeType::Cone:     return cone_.center;
		default:                 return Vector3::kZero;
		}
	}

	//Vector3 GetSize() const {
	//	switch (shapeType_) {
	//	case ShapeType::Sphere:   return Vector3(sphere_.radius);
	//	case ShapeType::Cube:     return cube_.size;
	//	case ShapeType::Cylinder: return Vector3(cylinder_.radius, cylinder_.height);
	//	case ShapeType::Cone:     return Vector3(cone_.radius, cone_.height);
	//	default:                 return Vector3::kZero;
	//	}
	//}
	//Vector3 GetRadius() const {
	//	switch (shapeType_) {
	//	case ShapeType::Sphere:   return Vector3(sphere_.radius);
	//	case ShapeType::Cube:     return cube_.size * 0.5f;
	//	case ShapeType::Cylinder: return Vector3(cylinder_.radius);
	//	case ShapeType::Cone:     return Vector3(cone_.radius);
	//	default:                 return Vector3::kZero;
	//	}
	//}


};


inline EffectEmitShape& EffectEmitShape::operator=(const EffectEmitShape& _shape) {
	shapeType_ = _shape.shapeType_;
	switch (shapeType_) {
	case ShapeType::Sphere:   sphere_   = _shape.sphere_;   break;
	case ShapeType::Cube:     cube_     = _shape.cube_;     break;
	//case ShapeType::Cylinder: cylinder_ = _shape.cylinder_; break;
	case ShapeType::Cone:     cone_     = _shape.cone_;     break;
	}

	return *this;
}
