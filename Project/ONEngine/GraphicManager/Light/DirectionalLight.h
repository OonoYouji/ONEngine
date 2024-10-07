#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "GameObjectManager/GameObjectManager.h"

using namespace Microsoft::WRL;

class DirectionalLight final : public BaseGameObject {
public:

	DirectionalLight() { CreateTag(this); }
	~DirectionalLight() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;


	/// ===================================================
	///	non overriding methods
	/// ===================================================

	/// <summary>
	/// commandListにconstantBufferをバインドする
	/// </summary>
	void BindToCommandList(
		UINT rootParamIndex,
		ID3D12GraphicsCommandList* commandList
	);

private:

	struct BufferData final {
		Vec4 color;
		Vec3 direction;
		float intencity;
	};

	ComPtr<ID3D12Resource> constantBuffer_;
	BufferData* bufferData_ = nullptr;

};