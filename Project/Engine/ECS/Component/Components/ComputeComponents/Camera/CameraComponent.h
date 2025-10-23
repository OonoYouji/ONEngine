#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"

/// @brief カメラの種類
enum class CameraType {
	Type3D, ///< 3Dカメラ
	Type2D, ///< 2Dカメラ
};

/// ///////////////////////////////////////////////////
/// カメラのコンポーネント
/// ///////////////////////////////////////////////////
class CameraComponent : public IComponent {
	friend class CameraUpdateSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CameraComponent();
	~CameraComponent() override;

	/// @brief ViewProjection行列の更新
	void UpdateViewProjection();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief ViewProjectionのBufferを作成
	/// @param _dxDevice Buffer作成に使うDxDevice
	void MakeViewProjection(class DxDevice* _dxDevice);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ConstantBuffer<ViewProjection> viewProjection_;

	float fovY_;
	float nearClip_;
	float farClip_;

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

	int cameraType_;
	bool isMainCameraRequest_;


public:
	/// ====================================================
	/// public : accessor
	/// ====================================================

	void SetIsMainCameraRequest(bool _isMainCamera);
	void SetFovY(float _fovY);
	void SetNearClip(float _nearClip);
	void SetFarClip(float _farClip);
	void SetCameraType(int _cameraType);


	bool GetIsMainCameraRequest() const;
	float GetFovY() const;
	float GetNearClip() const;
	float GetFarClip() const;
	int GetCameraType() const;

	bool IsMakeViewProjection() const;

	const ViewProjection& GetViewProjection() const;
	ConstantBuffer<ViewProjection>& GetViewProjectionBuffer();

	const Matrix4x4& GetViewMatrix() const;
	const Matrix4x4& GetProjectionMatrix() const;

};



/// @brief カメラ関連の数学関数群
namespace CameraMath {

	/// @brief perspective matrix の作成
	/// @param _fovY 視野角
	/// @param _aspectRatio アスペクト比 
	/// @param _nearClip 最小描画距離
	/// @param _farClip 最大描画距離
	/// @return 作成された perspective matrix
	Matrix4x4 MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip);

	/// @brief 平行投影行列の作成
	/// @param _left 左
	/// @param _right 右
	/// @param _bottom 下
	/// @param _top 上
	/// @param _znear 手前
	/// @param _zfar 奥行き
	/// @return 平行投影行列
	Matrix4x4 MakeOrthographicMatrix(float _left, float _right, float _bottom, float _top, float _znear, float _zfar);

}


/// Json変換
void from_json(const nlohmann::json& _j, CameraComponent& _c);
void to_json(nlohmann::json& _j, const CameraComponent& _c);

/// @brief Componentのデバッグ関数
namespace COMP_DEBUG {
	void CameraDebug(CameraComponent* _camera);
}