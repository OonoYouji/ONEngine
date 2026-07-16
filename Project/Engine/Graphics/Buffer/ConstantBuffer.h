#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"


/// /////////////////////////////////////////////////
/// 定数バッファクラス
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ConstantBuffer
 * @brief シェーダに渡すための定数データを保持・管理し、自動的なCPUメモリマッピングを内包する定数バッファテンプレートクラス
 * @tparam T 定数バッファに格納する構造体型
 */
template <typename T>
class ConstantBuffer final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	ConstantBuffer() : mappingData_(nullptr) {}
	
	/**
	 * @brief デストラクタ
	 */
	~ConstantBuffer() = default;

	/**
	 * @brief 定数バッファリソースの生成とCPUメモリ空間へのマッピング（Map）を行います。
	 * @param _dxDevice デバイスオブジェクトポインタ
	 */
	void Create(DxDevice* _dxDevice);

	/**
	 * @brief グラフィックスコマンドリストにこの定数バッファのGPU仮想アドレスをバインドします。
	 * @param _commandList 設定先のグラフィックスコマンドリスト
	 * @param _rootParameterIndex バインドするルートパラメータのインデックス
	 */
	void BindForGraphicsCommandList(ID3D12GraphicsCommandList* _commandList, UINT _rootParameterIndex) const;

	/**
	 * @brief コンピュートコマンドリストにこの定数バッファのGPU仮想アドレスをバインドします。
	 * @param _commandList 設定先のグラフィックスコマンドリスト
	 * @param _rootParameterIndex バインドするルートパラメータのインデックス
	 */
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

	/**
	 * @brief マップされたメモリ領域へCPU側のデータを書き込み（コピー）します。
	 * @param _mappingData コピー元データ構造体
	 */
	void SetMappedData(const T& _mappingData);

	/**
	 * @brief マップされたデータへの定数参照を取得します。
	 * @return データ構造体の定数参照
	 */
	const T& GetMappingData() const { return *mappingData_; }

	/**
	 * @brief 内部のID3D12Resourceを取得します。
	 * @return ID3D12Resourceポインタ
	 */
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
	if (mappingData_) {
		*mappingData_ = _mappingData;
	}
}

} /// ONEngine
