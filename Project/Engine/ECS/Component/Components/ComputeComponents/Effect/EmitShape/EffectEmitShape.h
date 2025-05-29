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

	Vector3 GetEmitDirection(const Vector3& _emitedPosition);

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

	void SetShapeType(ShapeType _type);

	void SetSphere(const Vector3& _center, float _radius);
	void SetSphere(const Sphere& _sphere);

	void SetCube(const Vector3& _center, const Vector3& _size);
	void SetCube(const Cube& _cube);

	void SetCone(const Vector3& _center, float _angle, float _radius, float _height);
	void SetCone(const Cone& _cone);

	Vector3 GetCenter() const;

	ShapeType GetType() const;

	Sphere GetSphere() const;
	Cube GetCube() const;
	Cone GetCone() const;

};


inline EffectEmitShape& EffectEmitShape::operator=(const EffectEmitShape& _shape) {
	shapeType_ = _shape.shapeType_;
	switch (shapeType_) {
	case ShapeType::Sphere: sphere_ = _shape.sphere_; break;
	case ShapeType::Cube: cube_ = _shape.cube_; break;
	case ShapeType::Cone: cone_ = _shape.cone_; break;
	}

	return *this;
}
