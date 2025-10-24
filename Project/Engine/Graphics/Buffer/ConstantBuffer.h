#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"


/// /////////////////////////////////////////////////
/// 定数バッファクラス
/// /////////////////////////////////////////////////
template <typename T>
class ConstantBuffer final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ConstantBuffer() = default;
	~ConstantBuffer() = default;

	/// @brief バッファの生成
	/// @param _dxDevice DxDeviceへのポインタ
	void Create(DxDevice* _dxDevice);

	/// @brief graphics pipeline にバインド
	/// @param _commandList ID3D12GraphicsCommandList
	/// @param _rootParameterIndex root parameter index
	void BindForGraphicsCommandList(ID3D12GraphicsCommandList* _commandList, UINT _rootParameterIndex) const;

	/// @brief compute pipeline にバインド
	/// @param _commandList ID3D12GraphicsCommandList
	/// @param _rootParameterIndex root parameter index
	void BindForComputeCommandList(ID3D12GraphicsCommandList* _commandList, UINT _rootParameterIndex) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	DxResource constantBuffer_;
	T* mappingData_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief mappingDataの設定
	/// @param _mappingData 設定するデータ
	void SetMappedData(const T& _mappingData);

	/// @brief mappingDataの取得
	/// @return 取得したデータ
	const T& GetMappingData() const { return *mappingData_; }

	/// @brief Resourceの取得
	/// @return リソースへのポインタ
	ID3D12Resource* Get() const { return constantBuffer_.Get(); }

};


template<typename T>
inline void ConstantBuffer<T>::Create(DxDevice* _dxDevice) {
	constantBuffer_.CreateResource(_dxDevice, sizeof(T));

	mappingData_ = nullptr;
	constantBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));
	SetMappedData(T{}); ///< 0クリア
}

template<typename T>
inline void ConstantBuffer<T>::BindForGraphicsCommandList(ID3D12GraphicsCommandList* _commandList, UINT _rootParameterIndex) const {
	_commandList->SetGraphicsRootConstantBufferView(_rootParameterIndex, constantBuffer_.Get()->GetGPUVirtualAddress());
}

template<typename T>
inline void ConstantBuffer<T>::BindForComputeCommandList(ID3D12GraphicsCommandList* _commandList, UINT _rootParameterIndex) const {
	_commandList->SetComputeRootConstantBufferView(_rootParameterIndex, constantBuffer_.Get()->GetGPUVirtualAddress());
}

template<typename T>
inline void ConstantBuffer<T>::SetMappedData(const T& _mappingData) {
	*mappingData_ = _mappingData;
}
