#pragma once

/// std
#include <memory>

/// externals
#include <jit/jit.h>
#include <metadata/class.h>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Quaternion.h"
#include "Engine/Core/Utility/Math/Vector3.h"


/// ===============================================
/// transform component
/// ===============================================
class Transform : public IComponent {
public:

	/// @brief 親子付けしているTransformの行列計算フラグ
	enum MatrixCalcFlag : int {
		kNone = 0,
		kPosition = 1 << 0, ///< positionのみを子に反映
		kRotate = 1 << 1, ///< rotateのみを子に反映
		kScale = 1 << 2, ///< scaleのみを子に反映
		kAll = kPosition | kRotate | kScale
	};


public:

	/// ===============================================
	/// public : sub class
	/// ===============================================

	struct BufferData final {
		Matrix4x4 matWorld;
	};


public:

	/// ===============================================
	/// public : methods
	/// ===============================================

	Transform();
	~Transform();

	void Update();

public:

	/// ===============================================
	/// public : objects
	/// ===============================================

	Vector3   position;
	Quaternion rotate;
	Vector3   scale;
	Matrix4x4 matWorld;

	int       matrixCalcFlags = kAll;


public:

	/// ===============================================
	/// public : accessor
	/// ===============================================

	/// @brief positionのセット
	/// @param _v position
	void SetPosition(const Vector3& _v);

	/// @brief position x のセット
	/// @param _x x座標
	void SetPositionX(float _x);

	/// @brief position y のセット
	/// @param _y y座標
	void SetPositionY(float _y);

	/// @brief position z のセット
	/// @param _z z座標
	void SetPositionZ(float _z);

	/// @brief rotateのセット
	/// @param _v rotate
	void SetRotate(const Vector3& _v);
	void SetRotate(const Quaternion& _q);

	/// @brief scaleのセット
	/// @param _v scale
	void SetScale(const Vector3& _v);

	/// @brief scale x のセット
	/// @param _x x軸拡縮
	void SetScaleX(float _x);

	/// @brief scale y のセット
	/// @param _y y軸拡縮
	void SetScaleY(float _y);

	/// @brief scale z のセット
	/// @param _z z軸拡縮
	void SetScaleZ(float _z);

	/// @brief positionを得る
	/// @return position 
	const Vector3& GetPosition() const;

	/// @brief rotateを得る
	/// @return rotate
	const Quaternion& GetRotate() const;

	/// @brief scaleを得る
	/// @return scale
	const Vector3& GetScale() const;

	/// @brief world 行列を得る
	/// @return world 行列
	const Matrix4x4& GetMatWorld() const;

};


namespace COMP_DEBUG {
	void TransformDebug(Transform* _transform);
}


/// =================================================
/// mono からのTransform取得用関数
/// =================================================

MonoObject* InternalGetTransform(uint32_t _entityId);
void InternalGetPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalGetLocalPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalGetRotate(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalGetScale(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalSetPosition(uint64_t _nativeHandle, float _x, float _y, float _z);
void InternalSetLocalPosition(uint64_t _nativeHandle, float _x, float _y, float _z);
void InternalSetRotate(uint64_t _nativeHandle, float _x, float _y, float _z);
void InternalSetScale(uint64_t _nativeHandle, float _x, float _y, float _z);