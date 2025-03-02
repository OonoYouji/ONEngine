#pragma once

/// std
#include <memory>

/// engine
#include "../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Vector3.h"


/// ===============================================
/// transform component
/// ===============================================
class Transform : public IComponent {
public:

	/// @brief 親子付けしているTransformの行列計算フラグ
	enum MatrixCalcFlag : int {
		kNone     = 0,
		kPosition = 1 << 0, ///< positionのみを子に反映
		kRotate   = 1 << 1, ///< rotateのみを子に反映
		kScale    = 1 << 2, ///< scaleのみを子に反映
		kAll      = kPosition | kRotate | kScale
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
	Vector3   rotate;
	Vector3   scale;
	Matrix4x4 matWorld;

	int       matrixCalcFlags = kAll;

private:

	/// ===============================================
	/// private : objects
	/// ===============================================

	Transform* parent_          = nullptr;


public:

	/// ===============================================
	/// public : accessor
	/// ===============================================

	/// @brief 親のセット
	/// @param _parent 親のtransform
	void SetParent(Transform* _parent) { parent_ = _parent; }



	/// @brief positionのセット
	/// @param _v position
	void SetPosition(const Vector3& _v) { position = _v; }

	/// @brief position x のセット
	/// @param _x x座標
	void SetPositionX(float _x) { position.x = _x; }

	/// @brief position y のセット
	/// @param _y y座標
	void SetPositionY(float _y) { position.y = _y; }

	/// @brief position z のセット
	/// @param _z z座標
	void SetPositionZ(float _z) { position.z = _z; }



	/// @brief rotateのセット
	/// @param _v rotate
	void SetRotate(const Vector3& _v) { rotate = _v; }

	/// @brief rotate x のセット
	/// @param _x x軸回転
	void SetRotateX(float _x) { rotate.x = _x; }

	/// @brief rotate y のセット
	/// @param _y y軸回転
	void SetRotateY(float _y) { rotate.y = _y; }

	/// @brief rotate z のセット
	/// @param _z z軸回転
	void SetRotateZ(float _z) { rotate.z = _z; }



	/// @brief scaleのセット
	/// @param _v scale
	void SetScale(const Vector3& _v) { scale = _v; }

	/// @brief scale x のセット
	/// @param _x x軸拡縮
	void SetScaleX(float _x) { scale.x = _x; }

	/// @brief scale y のセット
	/// @param _y y軸拡縮
	void SetScaleY(float _y) { scale.y = _y; }

	/// @brief scale z のセット
	/// @param _z z軸拡縮
	void SetScaleZ(float _z) { scale.z = _z; }



	/// @brief 親の取得
	/// @return 親のtransform
	const Transform* GetParent() const { return parent_; }

	/// @brief positionを得る
	/// @return position 
	const Vector3& GetPosition() const { return position; }

	/// @brief rotateを得る
	/// @return rotate
	const Vector3& GetRotate() const { return rotate; }

	/// @brief scaleを得る
	/// @return scale
	const Vector3& GetScale() const { return scale; }

	/// @brief world 行列を得る
	/// @return world 行列
	const Matrix4x4& GetMatWorld() const { return matWorld; }

};

