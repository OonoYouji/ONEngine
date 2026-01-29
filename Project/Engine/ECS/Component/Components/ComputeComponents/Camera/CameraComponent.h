#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"
#include <Engine/Core/Utility/Math/Vector3.h>


/// ----- 前方宣言 ----- ///
namespace ONEngine {

class CameraComponent;

/// @brief カメラの種類
enum class CameraType {
	Type3D, ///< 3Dカメラ
	Type2D, ///< 2Dカメラ
};


/// @brief Componentのデバッグ関数
namespace ComponentDebug {
void CameraDebug(CameraComponent* _camera);
}

/// Json変換
void from_json(const nlohmann::json& _j, CameraComponent& _c);
void to_json(nlohmann::json& _j, const CameraComponent& _c);


/// ///////////////////////////////////////////////////
/// カメラのコンポーネント
/// ///////////////////////////////////////////////////
class CameraComponent : public IComponent {
	/// ----- firend class ----- ///
	friend class CameraUpdateSystem;

	/// ----- friend function ----- ///
	friend void ComponentDebug::CameraDebug(CameraComponent* _camera);
	friend void from_json(const nlohmann::json& _j, CameraComponent& _c);
	friend void to_json(nlohmann::json& _j, const CameraComponent& _c);
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CameraComponent();
	~CameraComponent() override;

	/// @brief ViewProjection行列の更新
	void UpdateViewProjection();


	bool IsVisible(const Vector3& center, const Vector3& size) const;

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
	ConstantBuffer<Vector4> cameraPosBuffer_;

	float fovY_;
	float nearClip_;
	float farClip_;

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

	int cameraType_;
	bool isMainCameraRequest_;
	bool isDrawFrustum_;

	Vector2 orthographicSize_;

public:
	/// ====================================================
	/// public : accessor
	/// ====================================================

	void SetIsMainCameraRequest(bool _isMainCamera);
	void SetCameraType(int _cameraType);
	void SetOrthographicSize(const Vector2& _size);


	bool GetIsMainCameraRequest() const;
	int GetCameraType() const;

	bool IsMakeViewProjection() const;

	const ViewProjection& GetViewProjection() const;
	ConstantBuffer<ViewProjection>& GetViewProjectionBuffer();

	ConstantBuffer<Vector4>& GetCameraPosBuffer();

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


} /// ONEngine
