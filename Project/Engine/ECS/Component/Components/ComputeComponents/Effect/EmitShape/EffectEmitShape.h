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

	/// @brief エミッターの形状
	enum class ShapeType {
		Sphere,
		Cube,
		Cone,
	};

	/// @brief 形状1: 球
	struct Sphere {
		Vector3 center;
		float radius;
	};

	/// @brief 形状2: 立方体
	struct Cube {
		Vector3 center;
		Vector3 size;
	};

	/// @brief 形状3: 円錐
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

	/// @brief エミッターの形状の代入演算子
	/// @param _shape エミッターの形状
	/// @return 形状の参照
	EffectEmitShape& operator= (const EffectEmitShape& _shape);

	/// @brief エミッターの座標
	Vector3 GetEmitPosition();

	/// @brief エミット後の方向ベクトルを取得する
	/// @param _emitedPosition 出力後の座標
	/// @return エミット後の方向ベクトル
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

	const Sphere& GetSphere() const;
	const Cube& GetCube() const;
	const Cone& GetCone() const;

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
