#pragma once

/// std
#include <memory>
#include <vector>

/// engine
#include "Window.h"

/// ///////////////////////////////////////////////////
/// windowの管理クラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

class WindowManager final {
public:
	/// ===================================================
	/// public : enum
	/// ===================================================

	enum class WindowType {
		Main,
		Sub,
	};


public:
	/// ===================================================
	/// public : static method
	/// ===================================================

	/// @brief main windowのwindowプロシージャ
	/// @param _hwnd 
	/// @param _msg 
	/// @param _wparam 
	/// @param _lparam 
	/// @return 
	static LRESULT CALLBACK MainWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	/// @brief sub windowのwindowプロシージャ
	/// @param _hwnd 
	/// @param _msg 
	/// @param _wparam 
	/// @param _lparam 
	/// @return 
	static LRESULT CALLBACK SubWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);


public:
	/// ===================================================
	/// public : method
	/// ===================================================

	/**
	 * @brief コンストラクタ。DxManagerポインタを受け取ります。
	 * @param _dxm DxManagerポインタ
	 */
	WindowManager(DxManager* _dxm);

	/**
	 * @brief デストラクタ
	 */
	~WindowManager();

	/**
	 * @brief シングルトンインスタンスを取得します。
	 * @return WindowManagerインスタンスポインタ
	 */
	static WindowManager* GetInstance();

	/**
	 * @brief メインウィンドウを生成し、ウィンドウクラスとHWNDを初期化します。
	 */
	void Initialize();

	/**
	 * @brief 全ウィンドウのメッセージをポーリングし、更新処理を呼び出します。
	 */
	void Update();

	/**
	 * @brief 全ウィンドウをクリアし、破棄します。
	 */
	void Finalize();

	/**
	 * @brief メインウィンドウの描画前処理（ビューポート・レンダーターゲット切替など）を行います。
	 */
	void MainWindowPreDraw();

	/**
	 * @brief メインウィンドウの描画後処理を行います。
	 */
	void MainWindowPostDraw();

	/**
	 * @brief 全ウィンドウに対する描画前処理を一括実行します。
	 */
	void PreDrawAll();

	/**
	 * @brief 全ウィンドウに対する描画後処理を一括実行します。
	 */
	void PostDrawAll();

	/**
	 * @brief 全ウィンドウのスワップチェーンプレゼンテーションを一括実行します。
	 */
	void PresentAll();

	/**
	 * @brief 新しいウィンドウを生成して登録します。
	 * @param _windowName ウィンドウ名
	 * @param _windowSize ウィンドウのクライアントサイズ
	 * @param _windowType ウィンドウの種別（Main または Sub）
	 * @param _windowStyle ウィンドウのウィンドウスタイル（境界線、ボタン等）
	 * @return 生成されたウィンドウインスタンスのポインタ
	 */
	Window* GenerateWindow(const std::wstring& _windowName, const Vector2& _windowSize, WindowType _windowType = WindowType::Sub, UINT _windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));

	/**
	 * @brief 指定したウィンドウを非表示にします。
	 * @param _windowPtr 対象ウィンドウポインタ
	 */
	void HideGameWindow(Window* _windowPtr) { ShowWindow(_windowPtr->GetHwnd(), SW_HIDE); }

	/**
	 * @brief 指定したウィンドウを表示します。
	 * @param _windowPtr 対象ウィンドウポインタ
	 */
	void ShowGameWindow(Window* _windowPtr) { ShowWindow(_windowPtr->GetHwnd(), SW_SHOW); }

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief 実際にWin32ウィンドウを構築する内部関数です。
	 * @param _title ウィンドウタイトル
	 * @param _size クライアントサイズ
	 * @param _windowStyle スタイル
	 * @param _windowPtr 設定するウィンドウオブジェクトへのポインタ
	 * @param _windowType ウィンドウの種別
	 */
	void CreateGameWindow(const wchar_t* _title, const Vector2& _size, UINT _windowStyle, Window* _windowPtr, WindowType _windowType);

	/**
	 * @brief メインウィンドウ固有の毎フレーム更新処理。
	 */
	void UpdateMainWindow();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	///  ----- other class ----- ///
	class DxManager*                     pDxManager_ = nullptr;

	std::vector<std::unique_ptr<Window>> windows_; ///< 管理対象ウィンドウ群
	Window*                              pMainWindow_ = nullptr; ///< メインウィンドウのキャッシュポインタ

	bool                                 isProcessEnd_;
	bool                                 closeRequested_ = false; ///< アプリケーション終了要求フラグ


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief メインウィンドウのオブジェクトポインタを取得します。
	 * @return Windowポインタ
	 */
	Window* GetMainWindow() const;

	/**
	 * @brief 現在フォアグラウンドまたはアクティブになっているウィンドウを取得します。
	 * @return Windowポインタ
	 */
	Window* GetActiveWindow() const;

	/**
	 * @brief アプリケーションへの終了要求が発生しているか判定します。
	 * @return 終了要求があれば true、それ以外は false
	 */
	bool IsCloseRequested() const;

	/**
	 * @brief アプリケーションの終了要求フラグを明示的にセットします。
	 * @param _isCloseRequested セットする終了フラグ状態
	 */
	void SetCloseRequested(bool _isCloseRequested);

private:
	/// ===================================================
	/// private : copy delete
	/// ===================================================

	WindowManager(const WindowManager&)            = delete;
	WindowManager(WindowManager&&)                 = delete;
	WindowManager& operator=(const WindowManager&) = delete;
	WindowManager& operator=(WindowManager&&)      = delete;
};

/**
 * @brief C#（Mono）側からウィンドウクライアントサイズを取得するためのバインド関数。
 * @param _size サイズを格納するVector2ポインタ
 */
void InternalGetWindowSize(Vector2* _size);

} /// ONEngine
