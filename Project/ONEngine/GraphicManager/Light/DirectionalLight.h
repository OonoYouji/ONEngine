#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "GameObjectManager/GameObjectManager.h"

using namespace Microsoft::WRL;

/// ===================================================
/// DirectionalLight
/// ===================================================
class DirectionalLight final : public BaseGameObject {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	struct BufferData {
		Vec4  color;
		Vec3  direction;
		float intensity;
		int   active;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DirectionalLight() { CreateTag(this); }
	~DirectionalLight() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


private:

	/// ===================================================
	///	private : objects
	/// ===================================================

	BufferData data_;

	
public:
	
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief buffer dataの取得
	/// @return buffer data
	const BufferData& GetData() const { return data_; }

	/// @brief 色の設定
	/// @param _color 色
	void SetColor(const Vec4& _color);

	/// @brief 方向の設定
	/// @param _direction 方向
	void SetDirection(const Vec3& _direction);

	/// @brief 輝度の設定
	/// @param _intensity 輝度
	void SetIntensity(float _intensity);

};