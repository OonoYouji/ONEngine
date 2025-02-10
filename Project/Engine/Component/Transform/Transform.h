#pragma once

/// engine
#include "../IComponent.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Core/Utility/Math/Vector3.h"


/// ===============================================
/// transform component
/// ===============================================
class Transform : public IComponent {
public:
	
	/// ===============================================
	/// public : methods
	/// ===============================================

	Transform();
	~Transform();

	void Update();

private:

	/// ===============================================
	/// private : objects
	/// ===============================================

	Vector3    position_;
	Vector3    rotate_;
	Vector3    scale_;

	Matrix4x4  matWorld_;

	Transform* parent_ = nullptr;

public:

	/// ===============================================
	/// public : accessor
	/// ===============================================

	/// @brief 親のセット
	/// @param _parent 親のtransform
	void SetParent(Transform* _parent) { parent_ = _parent; }



	/// @brief positionのセット
	/// @param _v position
	void SetPosition(const Vector3& _v) { position_ = _v; }

	/// @brief position x のセット
	/// @param _x x座標
	void SetPositionX(float _x) { position_.x = _x; }

	/// @brief position y のセット
	/// @param _y y座標
	void SetPositionY(float _y) { position_.y = _y; }

	/// @brief position z のセット
	/// @param _z z座標
	void SetPositionZ(float _z) { position_.z = _z; }



	/// @brief rotateのセット
	/// @param _v rotate
	void SetRotate(const Vector3& _v) { rotate_ = _v; }

	/// @brief rotate x のセット
	/// @param _x x軸回転
	void SetRotateX(float _x) { rotate_.x = _x; }

	/// @brief rotate y のセット
	/// @param _y y軸回転
	void SetRotateY(float _y) { rotate_.y = _y; }

	/// @brief rotate z のセット
	/// @param _z z軸回転
	void SetRotateZ(float _z) { rotate_.z = _z; }



	/// @brief scaleのセット
	/// @param _v scale
	void SetScale(const Vector3& _v) { scale_ = _v; }

	/// @brief scale x のセット
	/// @param _x x軸拡縮
	void SetScaleX(float _x) { scale_.x = _x; }

	/// @brief scale y のセット
	/// @param _y y軸拡縮
	void SetScaleY(float _y) { scale_.y = _y; }

	/// @brief scale z のセット
	/// @param _z z軸拡縮
	void SetScaleZ(float _z) { scale_.z = _z; }



	/// @brief 親の取得
	/// @return 親のtransform
	const Transform* GetParent() const { return parent_; }

	/// @brief positionを得る
	/// @return position 
	const Vector3& GetPosition() const { return position_; }

	/// @brief rotateを得る
	/// @return rotate
	const Vector3& GetRotate() const { return rotate_; }

	/// @brief scaleを得る
	/// @return scale
	const Vector3& GetScale() const { return scale_; }

	/// @brief world 行列を得る
	/// @return world 行列
	const Matrix4x4& GetMatWorld() const { return matWorld_; }

};

