#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>

/// std
#include <vector>

/// engine
#include "../ComPtr/ComPtr.h"

/// /////////////////////////////////////////////////
/// dx12のスワップチェインを管理するクラス
/// /////////////////////////////////////////////////
namespace ONEngine {

class DxSwapChain {
public:

	/// @brief SwapChainのバッファ数(Double Buffering)
	static const int kBufferCount = 3;

public:
	/// ===================================================
	/// public : method
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	DxSwapChain();

	/**
	 * @brief デストラクタ。確保したRTVディスクリプタヒープのインデックス解放を行います。
	 */
	~DxSwapChain();
	
	/**
	 * @brief 指定されたウィンドウに紐付くDXGIスワップチェーンを生成し、バックバッファとRTVの登録、ビューポートとシザー矩形の設定を行います。
	 * @param _dxm デバイスやヒープを管理するDxManagerのポインタ
	 * @param _window 対象となるウィンドウオブジェクトのポインタ
	 */
	void Initialize(class DxManager* _dxm, class Window* _window);

	/**
	 * @brief ビューポートおよびシザー矩形設定を現在のアクティブなコマンドリストにバインドします。
	 * @param _commandList 設定先のグラフィックスコマンドリストポインタ
	 */
	void BindViewportAndScissorRectForCommandList(ID3D12GraphicsCommandList* _commandList) const;

	/**
	 * @brief 現在のアクティブなバックバッファに対し、リソース状態遷移（Transition）バリアを作成・設定します。
	 * @param _commandList コマンドリストポインタ
	 * @param _before 遷移前のリソース状態（D3D12_RESOURCE_STATES）
	 * @param _after 遷移後のリソース状態
	 */
	void CreateBarrier(ID3D12GraphicsCommandList* _commandList, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after);

	/**
	 * @brief 現在のアクティブなバックバッファレンダーターゲットを指定されたクリア色（デフォルトは黒）でクリアします。
	 * @param _commandList コマンドリストポインタ
	 */
	void ClearBackBuffer(ID3D12GraphicsCommandList* _commandList);

	/**
	 * @brief バックバッファとフロントバッファを交換（画面表示）します。
	 */
	void Present();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDXGISwapChain4>                  swapChain_  = nullptr;
	std::vector<ComPtr<ID3D12Resource>>      buffers_;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles_;
	std::vector<uint32_t>                    rtvIndices_;

	D3D12_VIEWPORT                           viewport_;
	D3D12_RECT                               scissorRect_;

	class DxManager*                         pDxManager_ = nullptr;
	class Window*                            pWindow_    = nullptr;

};


} /// ONEngine
