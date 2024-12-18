#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include "GameObjectManager/GameObjectManager.h"


/// ---------------------------------------------------
/// 投影方法
/// ---------------------------------------------------
enum PROJECTION_TYPE : uint8_t {
	PERSPECTIVE,	/// 透視投影
	ORTHOGRAPHIC,	/// 平行投影
};


/// ---------------------------------------------------
/// 特定の地点から地点に移動するためのデータ
/// ---------------------------------------------------
struct MoveData {
	Vec3 position;
	Vec3 rotate;
};



/// ===================================================
/// カメラの基底クラス
/// ===================================================
class BaseCamera : public BaseGameObject {
public:

	BaseCamera();
	virtual ~BaseCamera() = default;

	virtual void Initialize() = 0;
	virtual void Update()     = 0;

	void Debug() override;

public:

	void BaseInitialize();
	void BaseUpdate();

	void UpdateMatView();
	void UpdateMatPerspective();
	void UpdateMatOrthographic();

	void Transfer();

	const Mat4& GetMatView() const { return matView_; }
	const Mat4& GetMatProjection() const { return matProjection_; }
	const Mat4& GetMatVp() const { return matVp_; }

	ID3D12Resource* GetViewBuffer() const { return viewProjectionBuffer_.Get(); }

	ID3D12Resource* GetPositionBuffer() const { return cameraPostionBuffer_.Get(); }

	void SetProjectionType(PROJECTION_TYPE projectionType);

	void SetDistance(float _distance);

protected:

	float fovY_ = 0.45f;

	float nearZ_ = 0.1f;
	float farZ_  = 1000.0f;

	Mat4 matView_;
	Mat4 matProjection_;	/// 透視投影
	Mat4 matOrtho_;			/// 平行投影

	Mat4 matVp_;

	uint8_t projectionType_ = PROJECTION_TYPE::PERSPECTIVE;
	float distacene_ = 1.0f;


private:
	Microsoft::WRL::ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	Mat4* matVpData_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPostionBuffer_ = nullptr;
	Vec3* worldPosData = nullptr;
public:
	inline BaseCamera& operator=(const BaseCamera& other) = default;
};

