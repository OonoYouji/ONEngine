#pragma once

#include <string>

#include <Transform.h>

/// ===================================================
/// カメラの基底クラス
/// ===================================================
class BaseCamera {
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

	bool GetIsAcitve() const { return isActive_; }

protected:

	std::string name_ = "BaseCamera";
	bool isActive_ = true;

	Transform transform_{};

	float fovY_ = 0.45f;
	float farZ_ = 1000.0f;

	Mat4 matView_;
	Mat4 matProjection_;

	Mat4 matVp_;

public:
	inline BaseCamera& operator=(const BaseCamera& other) = default;
};