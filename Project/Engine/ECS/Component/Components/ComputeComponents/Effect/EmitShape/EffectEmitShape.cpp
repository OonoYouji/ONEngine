#include "EffectEmitShape.h"

using namespace ONEngine;

/// std
#include <numbers>

/// engine
#include "Engine/Core/Utility/Utility.h"

/**
 * @brief コンストラクタ。デフォルトで各形状（球・箱・コーン）の初期パラメータを設定します。
 */
EffectEmitShape::EffectEmitShape() {
	/// 球体の初期化
	sphere_.center = Vector3::Zero;
	sphere_.radius = 1.0f;
	/// 立方体の初期化
	cube_.center = Vector3::Zero;
	cube_.size = Vector3(1.0f, 1.0f, 1.0f);
	/// 円錐の初期化
	cone_.center = Vector3::Zero;
	cone_.angle = 30.0f;
	cone_.radius = 1.0f;
	cone_.height = 1.0f;
}

/**
 * @brief コピーコンストラクタ
 */
EffectEmitShape::EffectEmitShape(const EffectEmitShape& _shape) {
	shapeType_ = _shape.shapeType_;
	switch (shapeType_) {
	case ShapeType::Sphere:   sphere_ = _shape.sphere_;     break;
	case ShapeType::Cube:     cube_ = _shape.cube_;         break;
	case ShapeType::Cone:     cone_ = _shape.cone_;         break;
	}
}


/**
 * @brief エミッター形状の内部パラメータに基づき、ランダムな初期放出座標を算出します。
 */
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

/**
 * @brief 放出後の粒子が飛んでいく初期方向ベクトルを算出します。
 */
Vector3 EffectEmitShape::GetEmitDirection(const Vector3& _emitedPosition) {
	Vector3 direction = Vector3::Zero;
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

/**
 * @brief エミッターの形状タイプ（ShapeType）を設定します。
 */
void EffectEmitShape::SetShapeType(ShapeType _type) {
	shapeType_ = _type;
}

/**
 * @brief 球体形状のパラメータ（中心・半径）を直接設定します。
 */
void EffectEmitShape::SetSphere(const Vector3& _center, float _radius) {
	shapeType_ = ShapeType::Sphere;
	sphere_.center = _center;
	sphere_.radius = _radius;
}

/**
 * @brief 球体形状パラメータを設定します。
 */
void EffectEmitShape::SetSphere(const Sphere& _sphere) {
	shapeType_ = ShapeType::Sphere;
	sphere_ = _sphere;
}

/**
 * @brief 箱形状のパラメータ（中心・各軸サイズ）を直接設定します。
 */
void EffectEmitShape::SetCube(const Vector3& _center, const Vector3& _size) {
	shapeType_ = ShapeType::Cube;
	cube_.center = _center;
	cube_.size = _size;
}

/**
 * @brief 箱形状パラメータを設定します。
 */
void EffectEmitShape::SetCube(const Cube& _cube) {
	shapeType_ = ShapeType::Cube;
	cube_ = _cube;
}

/**
 * @brief コーン形状のパラメータ（中心・開口角・半径・高さ）を設定します。
 */
void EffectEmitShape::SetCone(const Vector3& _center, float _angle, float _radius, float _height) {
	shapeType_ = ShapeType::Cone;
	cone_.center = _center;
	cone_.angle = _angle;
	cone_.radius = _radius;
	cone_.height = _height;
}

/**
 * @brief コーン形状パラメータを設定します。
 */
void EffectEmitShape::SetCone(const Cone& _cone) {
	shapeType_ = ShapeType::Cone;
	cone_ = _cone;
}

/**
 * @brief 現在設定されている形状の中心座標を取得します。
 */
Vector3 EffectEmitShape::GetCenter() const {
	switch (shapeType_) {
	case ShapeType::Cube: return cube_.center;
	case ShapeType::Cone: return cone_.center;
	default: return Vector3::Zero;
	}
}

/**
 * @brief 形状タイプ（ShapeType）を取得します。
 */
EffectEmitShape::ShapeType EffectEmitShape::GetType() const {
	return shapeType_;
}

/**
 * @brief 球体パラメータ参照を取得します。
 */
const Sphere& EffectEmitShape::GetSphere() const {
	return sphere_;
}

/**
 * @brief 箱パラメータ参照を取得します。
 */
const Cube& EffectEmitShape::GetCube() const {
	return cube_;
}

/**
 * @brief コーンパラメータ参照を取得します。
 */
const Cone& EffectEmitShape::GetCone() const {
	return cone_;
}

