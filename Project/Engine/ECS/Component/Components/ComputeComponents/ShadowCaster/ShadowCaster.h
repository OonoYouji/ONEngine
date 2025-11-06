#pragma once

/// engine
#include "../../Interface/IComponent.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"


/// ///////////////////////////////////////////////////
/// 影の投影を行うためのコンポーネント
/// ///////////////////////////////////////////////////
class ShadowCaster : public IComponent {
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	ShadowCaster();
	~ShadowCaster() override;


	/// @brief ViewProjectionバッファの作成
	/// @param _dxDevice DxDeviceへのポインタ
	void CreateVPBuffer(DxDevice* _dxDevice);

	/// @brief DirectionalLightからライトビュー行列を設定
	/// @param _directionLight DirectionalLightへのポインタ
	void CalculationLightViewMatrix(class DirectionalLight* _directionLight);


private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	Matrix4x4 matLightView_;    ///< ライトビュー行列
	Matrix4x4 matLightProj_;    ///< ライト射影行列

	ConstantBuffer<ViewProjection> vpBuffer_;

	float nearClip_ = 0.1f;  ///< 最近クリップ距離
	float farClip_  = 1000.0f;///< 最遠クリップ距離

};

