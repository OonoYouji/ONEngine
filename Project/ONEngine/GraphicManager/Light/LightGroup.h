#pragma once

/// std
#include <memory>
#include <cstdint>

/// engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxStructuredBuffer.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"


class LightGroup final {
public:

	LightGroup();
	~LightGroup();

	void Initialize();
	void Update();

	/// <summary>
	/// DirectionalLightBufferをコマンドリストにバインド
	/// </summary>
	/// <param name="_rootParameterIndex"></param>
	/// <param name="_commandList"></param>
	void BindDirectionalLightBufferForCommandList(
		UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList
	);

	/// <summary>
	/// PointLightBufferをコマンドリストにバインド
	/// </summary>
	/// <param name="_rootParameterIndex"></param>
	/// <param name="_commandList"></param>
	void BindPointLightBufferForCommandList(
		UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList
	);


	void BindSpotLightBufferForCommandList(
		UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList
	);

private:

	/// 配置できるライトの最大数
	const size_t kMaxDirectionalLightNum_ = 5;
	const size_t kMaxPointLightNum_       = 5;
	const size_t kMaxSpotLightNum_        = 5;

	std::unique_ptr<DxStructuredBuffer<DirectionalLight::BufferData>> directionalLightBuffer_;
	std::unique_ptr<DxStructuredBuffer<PointLight::BufferData>>       pointLightBuffer_;
	std::unique_ptr<DxStructuredBuffer<SpotLight::BufferData>>        spotLightBuffer_;


public:

	void SetDirectionalLightBufferData(size_t _index, const DirectionalLight::BufferData& _data) {
		directionalLightBuffer_->SetMappedData(_index, _data);
	}

	void SetPointLightBufferData(size_t _index, const PointLight::BufferData& _data) {
		pointLightBuffer_->SetMappedData(_index, _data);
	}

	void SetSpotLightBufferData(size_t _index, const SpotLight::BufferData& _data) {
		spotLightBuffer_->SetMappedData(_index, _data);
	}

};