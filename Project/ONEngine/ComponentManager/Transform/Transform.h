#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <list>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

#include "ComponentManager/Base/BaseComponent.h"


/// ===================================================
/// 行列の計算方式のenum
/// ===================================================
enum ROTATE_ORDER : uint32_t {
	XZY, XYZ,
	YXZ, YZX,
	ZYX, ZXY,
	QUATERNION
};


/// ===================================================
/// オブジェクトのSRTを持つ
/// ===================================================
class Transform final : public BaseComponent {
public:

	Transform() { Initialize(); }
	~Transform() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	void Update() override;

	void Debug() override;

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

	void BindTransform(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex);

	void SetParent(Transform* parent);
	Transform* GetParent() const { return parent_; };

	void AddChild(Transform* child);
	std::list<Transform*> GetChilds() const {
		return childs_;
	}


private:

	Mat4 MakeRotate(uint32_t order);

public:

	Vec3 scale            = Vec3::kOne;
	Vec3 rotate           = {};
	Quaternion quaternion = {0, 0, 0, 1};
	Vec3 position         = {};

	Mat4 matTransform = Mat4::kIdentity;

	uint32_t rotateOrder = 1u;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> transformBuffer_ = nullptr;

	Transform*            parent_ = nullptr;
	std::list<Transform*> childs_;

private:
	Mat4* mapingData_ = nullptr;
public:
	inline Transform& operator= (const Transform&) = default;
};