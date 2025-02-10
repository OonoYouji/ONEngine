#pragma once

/// std
#include <memory>

/// engine
#include "../Interface/IEntity.h"

#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"


/// ===================================================
/// カメラ
/// ===================================================
class Camera : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Camera(class DxDevice* _dxDevice);
	~Camera();

	void Initialize() override;
	void Update()     override;


private:

	/// @brief 逆タンジェント
	/// @param _t 
	/// @return 逆タンジェント
	float Cot(float _t) const;

	/// @brief view matrix の作成
	/// @param _matWorld view matrix を作成するための world matrix
	/// @return 作成された view matrix
	Matrix4x4 MakeViewMatrix(const Matrix4x4& _matWorld) const;

	/// @brief perspective matrix の作成
	/// @param _fovY 視野角
	/// @param _aspectRatio アスペクト比 
	/// @param _nearClip 最小描画距離
	/// @param _farClip 最大描画距離
	/// @return 作成された perspective matrix
	Matrix4x4 MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip) const;

private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<ConstantBuffer<ViewProjection>> viewProjection_;

	float fovY_     = 0.0f;
	float nearClip_ = 0.0f;
	float farClip_  = 0.0f;

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

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


	float GetFovY()     const { return fovY_; }
	float GetNearClip() const { return nearClip_; }
	float GetFarClip()  const { return farClip_; }


	void SetFovY(float _fovY) { fovY_ = _fovY; }
	void SetNearClip(float _nearClip) { nearClip_ = _nearClip; }
	void SetFarClip(float _farClip) { farClip_ = _farClip; }



};

