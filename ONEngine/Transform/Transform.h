#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>



/// ===================================================
/// オブジェクトのSRTを持つ
/// ===================================================
class Transform final {
public:

	Transform() = default;
	~Transform() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

public:

	Vec3 scale = Vec3::kOne;
	Vec3 rotate = {};
	Vec3 position = {};

	Mat4 matTransform = Mat4::kIdentity;

public:
	inline Transform& operator= (const Transform&) = default;
};