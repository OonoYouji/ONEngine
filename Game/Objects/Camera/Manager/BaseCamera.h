#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>


/// ===================================================
/// カメラの基底クラス
/// ===================================================
class BaseCamera {
public:

	BaseCamera();
	~BaseCamera();

	void Initialize();

	const Mat4& GetMatView() const { return matView_; }
	const Mat4& GetMatProjection() const { return matProjection_; }

private:

	Vec3 position_{};
	Vec3 rotate_{};

	float fovY_;
	float farZ_;

	Mat4 matWorld_;
	Mat4 matView_;
	Mat4 matProjection_;

public:
	inline BaseCamera& operator=(const BaseCamera& other) = default;
};