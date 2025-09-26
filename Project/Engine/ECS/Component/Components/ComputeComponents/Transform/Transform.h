#pragma once

/// std
#include <memory>

/// externals
#include <jit/jit.h>
#include <metadata/class.h>
#include <nlohmann/json.hpp>

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
	void Reset() override;

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

	void SetPosition(const Vector3& _v);
	void SetRotate(const Vector3& _v);
	void SetRotate(const Quaternion& _q);
	void SetScale(const Vector3& _v);

	const Vector3& GetPosition() const;
	const Quaternion& GetRotate() const;
	const Vector3& GetScale() const;
	const Matrix4x4& GetMatWorld() const;

};


namespace COMP_DEBUG {
	void TransformDebug(Transform* _transform);
}

void from_json(const nlohmann::json& _j, Transform& _t);
void to_json(nlohmann::json& _j, const Transform& _t);


/// =================================================
/// mono からのTransform取得用関数
/// =================================================

void UpdateTransform(Transform* _transform);

void InternalGetPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalGetLocalPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalGetRotate(uint64_t _nativeHandle, float* _x, float* _y, float* _z, float* _w);
void InternalGetScale(uint64_t _nativeHandle, float* _x, float* _y, float* _z);
void InternalSetPosition(uint64_t _nativeHandle, float _x, float _y, float _z);
void InternalSetLocalPosition(uint64_t _nativeHandle, float _x, float _y, float _z);
void InternalSetRotate(uint64_t _nativeHandle, float _x, float _y, float _z, float _w);
void InternalSetScale(uint64_t _nativeHandle, float _x, float _y, float _z);