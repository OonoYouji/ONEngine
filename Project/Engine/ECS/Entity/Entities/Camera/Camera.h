#pragma once

/// std
#include <memory>

/// engine
//#include "../Interface/IEntity.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"

#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"

enum class CameraType {
	None,
	Type3D, ///< 3Dカメラ
	Type2D, ///< 2Dカメラ
};


/// ///////////////////////////////////////////////////
/// カメラ
/// ///////////////////////////////////////////////////
class Camera : public IEntity {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Camera(class DxDevice* _dxDevice);
	~Camera();

	void Initialize() override;
	void Update()     override;
	void UpdateViewProjection();

protected:

	/// @brief perspective matrix の作成
	/// @param _fovY 視野角
	/// @param _aspectRatio アスペクト比 
	/// @param _nearClip 最小描画距離
	/// @param _farClip 最大描画距離
	/// @return 作成された perspective matrix
	Matrix4x4 MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip) const;

	/// @brief 平行投影行列の作成
	/// @param _left 左
	/// @param _right 右
	/// @param _bottom 下
	/// @param _top 上
	/// @param _znear 手前
	/// @param _zfar 奥行き
	/// @return 平行投影行列
	Matrix4x4 MakeOrthographicMatrix(float _left, float _right, float _bottom, float _top, float _znear, float _zfar) const;


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unique_ptr<ConstantBuffer<ViewProjection>> viewProjection_;

	float fovY_     = 0.0f;
	float nearClip_ = 0.0f;
	float farClip_  = 0.0f;

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

	int cameraType_;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	void SetFovY(float _fovY) { fovY_ = _fovY; }
	void SetNearClip(float _nearClip) { nearClip_ = _nearClip; }
	void SetFarClip(float _farClip) { farClip_ = _farClip; }

	void SetCameraType(int _cameraType);


	/// @brief view projection の取得
	/// @return view projection data
	const ViewProjection& GetViewProjection() const {
		return viewProjection_->GetMappingData(); 
	}

	/// @brief view projection buffer の取得
	/// @return view projection buffer のポインタ
	ConstantBuffer<ViewProjection>* GetViewProjectionBuffer() {
		return viewProjection_.get();
	}

	const Matrix4x4& GetViewMatrix() { return matView_; }
	const Matrix4x4& GetProjectionMatrix() { return matProjection_; }

	float GetFovY()     const { return fovY_; }
	float GetNearClip() const { return nearClip_; }
	float GetFarClip()  const { return farClip_; }

	int GetCameraType() const { return cameraType_; }



};