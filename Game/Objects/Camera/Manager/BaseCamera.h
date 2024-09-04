#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <string>

#include <GameObjectManager.h>


/// ===================================================
/// カメラの基底クラス
/// ===================================================
class BaseCamera : public BaseGameObject {
public:

	BaseCamera();
	virtual ~BaseCamera() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;

public:

	void BaseInitialize();

	const Mat4& GetMatView() const { return matView_; }
	const Mat4& GetMatProjection() const { return matProjection_; }
	const Mat4& GetMatVp() const { return matVp_; }

	void UpdateMatView();
	void UpdateMatProjection();

	void Transfer();
	
	ID3D12Resource* GetViewBuffer() const { return viewProjectionBuffer_.Get(); }

protected:

	bool isActive_ = true;

	float fovY_ = 0.45f;
	float farZ_ = 1000.0f;

	Mat4 matView_;
	Mat4 matProjection_;

	Mat4 matVp_;


	Microsoft::WRL::ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	Mat4* matVpData_ = nullptr;

public:
	inline BaseCamera& operator=(const BaseCamera& other) = default;
};