#pragma once

/// directx
#include <d3d12.h>

/// std
#include <vector>
#include <string>
#include <optional>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Shader.h"

/// @brief RenderTargetViewのフォーマット
enum class RTVFormat {
	Color         = DXGI_FORMAT_R8G8B8A8_UNORM,     ///< 色
	WorldPosition = DXGI_FORMAT_R16G16B16A16_FLOAT, ///< ワールド座標
	Normal        = DXGI_FORMAT_R16G16B16A16_FLOAT, ///< 法線
	Flags         = DXGI_FORMAT_R32G32B32A32_FLOAT  ///< フラグ
};

/// @brief MRT用RenderTargetViewのインデックス
enum class RTVIndex {
	Color         = 0, ///< 色
	WorldPosition = 1, ///< ワールド座標
	Normal        = 2, ///< 法線
	Flags         = 3, ///< フラグ
	Count /// 要素数
};


/// ///////////////////////////////////////////////////
/// グラフィクス用	pipeline
/// ///////////////////////////////////////////////////
namespace ONEngine {

class GraphicsPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	GraphicsPipeline();

	/**
	 * @brief デストラクタ
	 */
	~GraphicsPipeline();

	/**
	 * @brief これまでに設定したシェーダ、レイアウト、各種ブレンド・ラスタライザ等の情報をもとに、D3D12ルートシグネチャおよびパイプラインステート（PSO）を新規生成します。
	 * @param _dxDevice デバイスポインタ
	 */
	void CreatePipeline(class DxDevice* _dxDevice);


	/*--- root signature ---*/

	/**
	 * @brief 使用するシェーダオブジェクト（コンパイル済みの各ステージBlobを内包）を登録します。
	 * @param _shader 設定するShaderオブジェクトポインタ
	 */
	void SetShader(Shader* _shader);

	/**
	 * @brief 頂点入力レイアウト要素（InputElement）を追加します。
	 * @param _semanticName セマンティクス名（例: "POSITION", "TEXCOORD" など）
	 * @param _semanticIndex セマンティクスのインデックス
	 * @param _format 画素・データの形式
	 * @param _inputSlot 頂点バッファのバインドスロット
	 */
	void AddInputElement(const std::string& _semanticName, uint32_t _semanticIndex, DXGI_FORMAT _format, UINT _inputSlot = 0u);

	/**
	 * @brief ルートパラメータに定数バッファビュー（CBV）の直接記述スロットを追加します。
	 * @param _shaderVisibility 対象となるシェーダステージ
	 * @param _shaderRegister HLSLレジスタ番号（register(bXX)のXX部）
	 */
	void AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

	/**
	 * @brief ルートパラメータにシェーダリソースビュー（SRV）の直接記述スロットを追加します。
	 * @param _shaderVisibility 対象となるシェーダステージ
	 * @param _shaderRegister HLSLレジスタ番号（register(tXX)のXX部）
	 */
	void AddSRV(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

	/**
	 * @brief ルートパラメータに32ビットのインライン定数（Root Constants）を追加します。
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
	 * @param _descriptorIndex  ディスクリプタレンジの配列インデックス
	 */
	void AddDescriptorTable(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _descriptorIndex);

	/**
	 * @brief ルートパラメータに静的サンプラ（デフォルトのClamp等）を追加します。
	 * @param _shaderVisibility 対象シェーダ
	 * @param _shaderRegister レジスタ番号（register(sXX)）
	 */
	void AddStaticSampler(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

	/**
	 * @brief 詳細なサンプラ記述子を指定して静的サンプラを追加します。
	 */
	void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& _samplerDesc, D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

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
	 * @brief ラスタライザステートを設定します。
	 * @param _desc ラスタライザ設定構造体
	 */
	void SetRasterizerDesc(const D3D12_RASTERIZER_DESC& _desc);

	/**
	 * @brief 深度ステンシルステートを設定します。
	 * @param _desc 深度ステンシル設定構造体
	 */
	void SetDepthStencilDesc(const D3D12_DEPTH_STENCIL_DESC& _desc);

	/**
	 * @brief レンダーターゲットカラーのブレンドモード設定を適用します。
	 * @param _desc 設定するブレンド設定構造体
	 */
	void SetBlendDesc(const D3D12_BLEND_DESC& _desc);

	/**
	 * @brief 同時出力するレンダーターゲット（RTV）の最大数を設定します。
	 * @param _rtvNum RTVバッファ数
	 */
	void SetRTVNum(uint32_t _rtvNum);

	/**
	 * @brief レンダーターゲットの画素フォーマットを一括設定します。
	 * @param _rtvFormats フォーマットのリスト
	 */
	void SetRTVFormats(const std::vector<DXGI_FORMAT>& _rtvFormats);

	/**
	 * @brief 特定のインデックスのレンダーターゲット画素フォーマットを設定します。
	 * @param _rtvFormat 設定するフォーマット
	 * @param _rtvIndex 設定対象のレンダーターゲットインデックス
	 */
	void SetRTVFormat(DXGI_FORMAT _rtvFormat, uint32_t _rtvIndex);

	/**
	 * @brief コマンドリストにパイプラインステートおよびルートシグネチャをバインドします。
	 * @param _dxCommand コマンド管理者ポインタ
	 */
	void SetPipelineStateForCommandList(class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief ルートシグネチャ（ID3D12RootSignature）を内部生成します。
	 */
	void CreateRootSignature(class DxDevice* _dxDevice);

	/**
	 * @brief グラフィックスパイプラインステート（PSO）を内部生成します。
	 */
	void CreatePipelineStateObject(class DxDevice* _dxDevice);

	/**
	 * @brief メッシュ・アンプリフィケーションシェーダを含んだメッシュシェーダPSOの内部生成を行います。
	 */
	void CreateMeshPipelineStateObject(class DxDevice* _dxDevice);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature>             rootSignature_;
	ComPtr<ID3D12PipelineState>             pipelineState_;


	/// parameters
	std::vector<D3D12_INPUT_ELEMENT_DESC>   inputElements_;
	std::vector<std::string>                semanticNames_;

	D3D12_RASTERIZER_DESC                   rasterizerDesc_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE           primitiveTopologyType_;
	D3D12_BLEND_DESC                        blendDesc_;

	std::vector<D3D12_ROOT_PARAMETER>       rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC>  staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>     descriptorRanges_;

	Shader* pShader_;


	/// pipeline settings
	std::optional<D3D12_DEPTH_STENCIL_DESC> depthStancilDesc_;
	uint32_t                                rtvNum_ = 1;
	std::vector<DXGI_FORMAT>                rtvFormats_;
};

/**
 * @brief 通常の深度書き込み・比較テストを有効にしたデフォルトのD3D12_DEPTH_STENCIL_DESCを取得します。
 * @return 深度ステンシル設定
 */
D3D12_DEPTH_STENCIL_DESC DefaultDepthStencilDesc();

/**
 * @brief 深度テスト・深度書き込みを完全に無効化したD3D12_DEPTH_STENCIL_DESCを取得します。
 * @return 深度ステンシル設定
 */
D3D12_DEPTH_STENCIL_DESC DepthNone();

/**
 * @brief テレグラフ（予兆線）描画等に適した特殊な深度ステンシル設定を取得します。
 * @return 深度ステンシル設定
 */
D3D12_DEPTH_STENCIL_DESC TelegraphDepthStencilDesc();

/**
 * @brief 深度テストのみを有効（読み取り専用）にし、書き込みを無効化したD3D12_DEPTH_STENCIL_DESCを取得します。
 * @return 深度ステンシル設定
 */
D3D12_DEPTH_STENCIL_DESC DepthRead();


/**
 * @namespace BlendMode
 * @brief 主要なブレンドステート（通常アルファブレンド、加算、減算、乗算、スクリーン）を生成する関数を提供する名前空間
 */
namespace BlendMode {
	/**
	 * @brief 通常のアルファブレンド（SrcAlpha * Src + (1 - SrcAlpha) * Dest）設定を取得します。
	 */
	D3D12_BLEND_DESC Normal();

	/**
	 * @brief 加算合成（One * Src + One * Dest）設定を取得します。
	 */
	D3D12_BLEND_DESC Add();

	/**
	 * @brief 減算合成設定を取得します。
	 */
	D3D12_BLEND_DESC Subtract();

	/**
	 * @brief 乗算合成設定を取得します。
	 */
	D3D12_BLEND_DESC Multiply();

	/**
	 * @brief スクリーン合成設定を取得します。
	 */
	D3D12_BLEND_DESC Screen();

	/**
	 * @brief ブレンドを完全に無効化（不透明）する設定を取得します。
	 */
	D3D12_BLEND_DESC None();
}

/**
 * @namespace StaticSampler
 * @brief 静的サンプラ設定を提供する名前空間
 */
namespace StaticSampler {
	/**
	 * @brief 境界線をクランプ（Clamp）する標準的なテクスチャサンプラ設定を取得します。
	 */
	D3D12_STATIC_SAMPLER_DESC ClampSampler();
}

} /// ONEngine
