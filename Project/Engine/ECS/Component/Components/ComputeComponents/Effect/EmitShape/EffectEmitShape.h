#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"

/// /////////////////////////////////////////////////////////////////
/// エフェクトの発生形状を指定するクラス
/// /////////////////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class EffectEmitShape
 * @brief エフェクト粒子の放出開始位置や方向（球・箱・コーン等）を規定するエミッター形状クラス
 */
class EffectEmitShape final {
public:
	/// =================================================
	/// public : sub class
	/// =================================================

	/**
	 * @enum ShapeType
	 * @brief エミッターの幾何形状タイプ
	 */
	enum class ShapeType {
		Sphere, ///< 球体
		Cube,   ///< 立方体・直方体
		Cone,   ///< 円錐体
	};

public:
	/// =================================================
	/// public : methods
	/// =================================================

	/**
	 * @brief コンストラクタ
	 */
	EffectEmitShape();
	/**
	 * @brief コピーコンストラクタ
	 */
	EffectEmitShape(const EffectEmitShape& _shape);
	/**
	 * @brief デストラクタ
	 */
	~EffectEmitShape() = default;

	/**
	 * @brief コピー代入演算子
	 */
	EffectEmitShape& operator= (const EffectEmitShape& _shape);

	/**
	 * @brief エミッター形状の内部パラメータに基づき、ランダムな初期放出座標を算出します。
	 */
	Vector3 GetEmitPosition();

	/**
	 * @brief 放出後の粒子が飛んでいく初期方向ベクトルを算出します。
	 * @param _emitedPosition 放出座標
	 * @return 放出方向（正規化済み）
	 */
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

	/**
	 * @brief エミッターの形状タイプ（ShapeType）を設定します。
	 */
	void SetShapeType(ShapeType _type);

	/**
	 * @brief 球体形状のパラメータ（中心・半径）を直接設定します。
	 */
	void SetSphere(const Vector3& _center, float _radius);
	/**
	 * @brief 球体形状パラメータを設定します。
	 */
	void SetSphere(const Sphere& _sphere);

	/**
	 * @brief 箱形状のパラメータ（中心・各軸サイズ）を直接設定します。
	 */
	void SetCube(const Vector3& _center, const Vector3& _size);
	/**
	 * @brief 箱形状パラメータを設定します。
	 */
	void SetCube(const Cube& _cube);

	/**
	 * @brief コーン形状のパラメータ（中心・開口角・半径・高さ）を設定します。
	 */
	void SetCone(const Vector3& _center, float _angle, float _radius, float _height);
	/**
	 * @brief コーン形状パラメータを設定します。
	 */
	void SetCone(const Cone& _cone);

	/**
	 * @brief 現在設定されている形状の中心座標を取得します。
	 */
	Vector3 GetCenter() const;

	/**
	 * @brief 形状タイプ（ShapeType）を取得します。
	 */
	ShapeType GetType() const;

	/**
	 * @brief 球体パラメータ参照を取得します。
	 */
	const Sphere& GetSphere() const;
	/**
	 * @brief 箱パラメータ参照を取得します。
	 */
	const Cube& GetCube() const;
	/**
	 * @brief コーンパラメータ参照を取得します。
	 */
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

} /// ONEngine
