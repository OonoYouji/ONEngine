#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <GameObjectManager.h>

enum CAMEAR_SHAKE_TYPE : uint32_t {
	LOCAL_AXIS_X,		/// ローカルのx軸でシェイク
	LOCAL_AXIS_Y,		/// ローカルのy軸でシェイク
	LOCAL_AXIS_Z,		/// ローカルのz軸でシェイク
	GLOBAL_AXIS_Y,		/// グローバルのy軸でシェイク
	CAMERA_SHAKE_TYPE_COUNT,
};


/// ===================================================
/// カメラの基底クラス
/// ===================================================
class BaseCamera : public BaseGameObject {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

public:

	BaseCamera();
	virtual ~BaseCamera() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;

public:

	void BaseInitialize();

	void UpdateMatView();
	void UpdateMatProjection();

	void Transfer();

	const Mat4& GetMatView() const { return matView_; }
	const Mat4& GetMatProjection() const { return matProjection_; }
	const Mat4& GetMatVp() const { return matVp_; }

	ID3D12Resource* GetViewBuffer() const { return viewProjectionBuffer_.Get(); }

protected:

	bool isActive_ = true;

	float fovY_ = 0.45f;
	float farZ_ = 1000.0f;

	Mat4 matView_;
	Mat4 matProjection_;
	Mat4 matVp_;

	class Shake* shake_ = nullptr;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	Mat4* matVpData_ = nullptr;
public:
	inline BaseCamera& operator=(const BaseCamera& other) = default;
};



class Shake : public BaseGameObject {
public:
	Shake() { CreateTag(this); }
	~Shake() {}
	void Initialize() override;
	void Update() override;
	void Debug() override;
	void Setting(float time, float speed, float amplitude);
private:
	float time_;
	float speed_;
	float amplitude_;
};