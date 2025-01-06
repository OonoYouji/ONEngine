#include "LightGroup.h"

LightGroup::LightGroup() {}
LightGroup::~LightGroup() {}

void LightGroup::Initialize() {

	directionalLightBuffer_.reset(new DxStructuredBuffer<DirectionalLight::BufferData>());
	directionalLightBuffer_->Create(kMaxDirectionalLightNum_);

	pointLightBuffer_.reset(new DxStructuredBuffer<PointLight::BufferData>());
	pointLightBuffer_->Create(kMaxPointLightNum_);

	/// 仮のデータで初期化

	for(uint32_t i = 0; i < kMaxDirectionalLightNum_; ++i) {
		DirectionalLight::BufferData data = {
			.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.direction = Vec3(-0.98f, 0.0f, 0.195f).Normalize(),
			.intencity = 1.0f,
			//.isActive  = false
		};

		directionalLightBuffer_->SetMappedData(i, data);
	}
	
	for(uint32_t i = 0; i < kMaxPointLightNum_; ++i) {
		PointLight::BufferData data = {
			.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.position  = Vec3(0.0f, 10.0f, 0.0f),
			.intencity = 1.0f,
			.radius    = 1.0f,
			.decay     = 1.0f,
			.active    = 0
		};

		pointLightBuffer_->SetMappedData(i, data);
	}
}


void LightGroup::BindDirectionalLightBufferForCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	directionalLightBuffer_->BindToCommandList(_rootParameterIndex, _commandList);
}

void LightGroup::BindPointLightBufferForCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	pointLightBuffer_->BindToCommandList(_rootParameterIndex, _commandList);
}
