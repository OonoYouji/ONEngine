#pragma once

/// windows
#include <Windows.h>

/// std
#include <string>
#include <memory>

/// engine
#include "Engine/Core/DirectX12/SwapChain/DxSwapChain.h"
#include "Engine/Core/Utility/Math/Vector2.h"



/// ///////////////////////////////////////////////////
/// windowクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class Window
 * @brief Win32アプリケーションウィンドウのライフサイクルとDirectX12スワップチェーンを管理するクラス
 */
class Window {
	friend class WindowManager;
public:
	/// ===================================================
	/// public : method
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Window();

	/**
	 * @brief デストラクタ
	 */
	~Window();

	/**
	 * @brief ウィンドウクラスの登録、ウィンドウの生成、およびスワップチェーンの初期化を行います。
	 * @param _windowName ウィンドウのタイトル（ワイド文字列）
	 * @param _windowSize ウィンドウのクライアント領域サイズ
	 * @param _dxm DirectX12デバイスなどを管理するDxManagerのポインタ
	 */
	void Initialize(const std::wstring& _windowName, const Vector2& _windowSize, class DxManager* _dxm);

	/**
	 * @brief 描画前処理。レンダーターゲットビューの切り替え等を行います。
	 */
	void PreDraw();

	/**
	 * @brief 描画後処理。リソースバリアのトランジションなどを行います。
	 */
	void PostDraw();

	/**
	 * @brief ウィンドウメッセージのポーリングおよび処理を行います。
	 */
	void Update();

	/**
	 * @brief スワップチェーンのPresentを実行し、描画内容を画面に表示します。
	 */
	void Present();

	/**
	 * @brief ウィンドウが現在開いており、破棄されていない状態か判定します。
	 * @return 開いていれば true、閉じられていれば false
	 */
	bool IsOpenWindow();

	/**
	 * @brief フルスクリーンとウィンドウモードの切り替えを行います。
	 */
	void ToggleFullScreen();

private:
	/// ===================================================
	/// public : objects
	/// ===================================================

	/// ----- other class ----- ///
	class DxManager* pDxManager_;


	std::wstring                 windowName_;
	Vector2                      windowSize_;

	WNDCLASS                     windowClass_;
	RECT                         wrc_;
	RECT                         fullscreenRect_;
	HWND                         hwnd_;
	MSG                          msg_;
	UINT                         windowStyle_;

	UINT                         processMessage_;

	std::unique_ptr<DxSwapChain> dxSwapChain_;

	bool isFullScreen_ = false;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief ウィンドウハンドル（HWND）を取得します。
	 * @return HWND
	 */
	HWND GetHwnd() const;

	/**
	 * @brief 登録されたウィンドウクラス（WNDCLASS）を取得します。
	 * @return WNDCLASS
	 */
	const WNDCLASS& GetWNDCLASS() const;

	/**
	 * @brief 最新の更新処理で処理されたウィンドウメッセージIDを取得します。
	 * @return メッセージID
	 */
	UINT GetProcessMessage() const;

	/**
	 * @brief ウィンドウの現在のクライアントサイズを取得します。
	 * @return ウィンドウサイズベクトル (Vector2)
	 */
	const Vector2& GetWindowSize() const;


private:
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator= (const Window&) = delete;
	Window& operator= (Window&&) = delete;
};


} /// ONEngine
