#include "LightGroup.h"

/// std
#include <numbers>


LightGroup::LightGroup() {}
LightGroup::~LightGroup() {}

void LightGroup::Initialize() {

	directionalLightBuffer_.reset(new DxStructuredBuffer<DirectionalLight::BufferData>());
	directionalLightBuffer_->Create(static_cast<uint32_t>(kMaxDirectionalLightNum_));

	pointLightBuffer_.reset(new DxStructuredBuffer<PointLight::BufferData>());
	pointLightBuffer_->Create(static_cast<uint32_t>(kMaxPointLightNum_));

	spotLightBuffer_.reset(new DxStructuredBuffer<SpotLight::BufferData>());
	spotLightBuffer_->Create(static_cast<uint32_t>(kMaxSpotLightNum_));


	/// 仮のデータで初期化
	for(size_t i = 0; i < kMaxDirectionalLightNum_; ++i) {
		DirectionalLight::BufferData data = {
			.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.direction = Vec3(-0.98f, 0.0f, 0.195f).Normalize(),
			.intencity = 1.0f,
			//.isActive  = false
		};

		directionalLightBuffer_->SetMappedData(i, data);
	}
	
	for(size_t i = 0; i < kMaxPointLightNum_; ++i) {
		PointLight::BufferData data = {
			.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.position  = Vec3(0.0f, 10.0f, 0.0f),
			.intensity = 1.0f,
			.radius    = 1.0f,
			.decay     = 1.0f,
			.active    = 0
		};

		pointLightBuffer_->SetMappedData(i, data);
	}

	for (size_t i = 0; i < kMaxSpotLightNum_; ++i) {
		SpotLight::BufferData data = {
			.color     = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.position  = Vec3(0.0f, 10.0f, 0.0f),
			.intensity = 1.0f,
			.direction = Vec3(0.0f, -1.0f, 0.0f),
			.distance  = 10.0f,
			.decay     = 2.0f,
			.cosAngle  = std::numbers::pi_v<float> / 3.0f,
			.active    = 0
		};

		spotLightBuffer_->SetMappedData(i, data);
	}
}

void LightGroup::Update() {
	
	/// lightのデータを更新して、あるはずのないlightを無効化

	for (size_t i = 0; i < kMaxDirectionalLightNum_; ++i) {
		DirectionalLight::BufferData data = directionalLightBuffer_->GetMappedData(i);
		data.active = 0;
		directionalLightBuffer_->SetMappedData(i, data);
	}

	for (size_t i = 0; i < kMaxPointLightNum_; ++i) {
		PointLight::BufferData data = pointLightBuffer_->GetMappedData(i);
		data.active = 0;
		pointLightBuffer_->SetMappedData(i, data);
	}

	for (size_t i = 0; i < kMaxSpotLightNum_; ++i) {
		SpotLight::BufferData data = spotLightBuffer_->GetMappedData(i);
		data.active = 0;
		spotLightBuffer_->SetMappedData(i, data);
	}

}


void LightGroup::BindDirectionalLightBufferForCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	directionalLightBuffer_->BindToCommandList(_rootParameterIndex, _commandList);
}

void LightGroup::BindPointLightBufferForCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	pointLightBuffer_->BindToCommandList(_rootParameterIndex, _commandList);
}

void LightGroup::BindSpotLightBufferForCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	spotLightBuffer_->BindToCommandList(_rootParameterIndex, _commandList);
}
