#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "GameObjectManager/GameObjectManager.h"

using namespace Microsoft::WRL;

class DirectionalLight final : public BaseGameObject {
public:

	struct BufferData {
		Vec4 color;
		Vec3 direction;
		float intencity;
		int   active;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DirectionalLight() { CreateTag(this); }
	~DirectionalLight() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;


private:

	/// ===================================================
	///	private : objects
	/// ===================================================

	BufferData data_;

	
public:
	
	/// ===================================================
	/// public : accessor
	/// ===================================================


	void SetColor(const Vec4& color) { data_.color = color; }

	void SetDirection(const Vec3& direction);

	void SetIntencity(float intencity) { data_.intencity = intencity; }

	const BufferData& GetData() const { return data_; }

};