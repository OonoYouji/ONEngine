#pragma once

/// directX
#include <d3d12.h>

/// std
#include <vector>
#include <string>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Shader.h"

/// ///////////////////////////////////////////////////
/// ComputePipeline
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ComputePipeline
 * @brief コンピュートシェーダ（CS）用パイプライン（ルートシグネチャ、パイプラインステートオブジェクト(PSO)）を定義・生成・バインドするためのクラス
 */
class ComputePipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	ComputePipeline();

	/**
	 * @brief デストラクタ
	 */
	~ComputePipeline();

	/**
	 * @brief 登録されたコンピュートシェーダおよびルートパラメータ設定をもとに、ルートシグネチャとコンピュートPSO（Compute PSO）を生成します。
	 * @param _dxDevice デバイスポインタ
	 */
	void CreatePipeline(class DxDevice* _dxDevice);


	/*--- root signature ---*/

	/**
	 * @brief 使用するシェーダオブジェクト（CSステージのBlobを保持）を登録します。
	 * @param _shader 設定するShaderオブジェクトポインタ
	 */
	void SetShader(Shader* _shader);

	/**
	 * @brief ルートパラメータに定数バッファビュー（CBV）の直接記述スロットを追加します。
	 * @param _shaderVisibility 対象となるシェーダステージ
	 * @param _shaderRegister HLSLレジスタ番号（register(bXX)のXX部）
	 */
	void AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

	/**
	 * @brief ルートパラメータに32ビットのインライン定数（Root Constants）を追加します。
	 * @param _shaderVisibility 対象シェーダ
	 * @param _shaderRegister HLSLレジスタ番号
	 * @param _num32bitValue 転送する32ビット値の個数
	 */
	void Add32BitConstant(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister, uint32_t _num32bitValue = 1u);

	/**
	 * @brief ディスクリプタテーブル用のレンジ（連続したビュー範囲）を定義して内部リストに追加します。
	 * @param _baseShaderRegister 先頭のHLSLレジスタ番号
	 * @param _numDescriptor この範囲で確保するディスクリプタの総数
	 * @param _rangeType レンジの形式（CBV, SRV, UAV）
	 */
	void AddDescriptorRange(uint32_t _baseShaderRegister, uint32_t _numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE  _rangeType);

	/**
	 * @brief ルートパラメータに、追加済みのディスクリプタレンジを参照するディスクリプタテーブルスロットを追加します。
	 * @param _shaderVisibility 対象シェーダステージ
	 * @param _descriptorIndex バインドするディスクリプタレンジの配列インデックス
	 */
	void AddDescriptorTable(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _descriptorIndex);

	/**
	 * @brief ルートパラメータに静的サンプラを追加します。
	 * @param _shaderVisibility 対象シェーダ
	 * @param _shaderRegister レジスタ番号（register(sXX)）
	 * @param _isComparisonSampler 比較（シャドウマップ参照等）用サンプラにするかどうか
	 */
	void AddStaticSampler(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister, bool _isComparisonSampler = false);

	/**
	 * @brief ラスタライザのフィルモードを設定します。
	 */
	void SetFillMode(D3D12_FILL_MODE _fillMode);

	/**
	 * @brief ラスタライザのカリングモードを設定します。
	 */
	void SetCullMode(D3D12_CULL_MODE _cullMode);

	/**
	 * @brief プリミティブトポロジーのトポロジータイプを設定します。
	 */
	void SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE _topologyType);


	/*--- pipeline state ---*/

	/**
	 * @brief コマンドリストにコンピュートPSOおよびルートシグネチャをバインドします。
	 * @param _dxCommand コマンド管理者ポインタ
	 */
	void SetPipelineStateForCommandList(class DxCommand* _dxCommand);


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief ルートシグネチャを内部生成します。
	 */
	void CreateRootSignature(class DxDevice* _dxDevice);

	/**
	 * @brief コンピュートパイプラインステート（Compute PSO）を内部生成します。
	 */
	void CreatePipelineStateObject(class DxDevice* _dxDevice);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Shader* shader_;

	ComPtr<ID3D12RootSignature>             rootSignature_;
	ComPtr<ID3D12PipelineState>             pipelineState_;

	D3D12_RASTERIZER_DESC                   rasterizerDesc_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE           primitiveTopologyType_;
	D3D12_BLEND_DESC                        blendDesc_;

	std::vector<D3D12_ROOT_PARAMETER>       rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC>  staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>     descriptorRanges_;
};


} /// ONEngine
