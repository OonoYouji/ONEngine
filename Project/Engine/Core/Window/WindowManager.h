#pragma once

/// std
#include <memory>
#include <vector>

/// engine
#include "Window.h"

/// ===================================================
/// windowの管理クラス
/// ===================================================
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

	WindowManager(class DxManager* _dxManager);
	~WindowManager();

	void Initialize();
	void Update();
	void Finalize();

	/// @brief main windowの描画前処理
	void MainWindowPreDraw();

	/// @brief main windowの描画後処理
	void MainWindowPostDraw();

	/// @brief windows_の描画前処理
	void PreDrawAll();

	/// @brief windows_の描画後処理
	void PostDrawAll();

	/// @brief windows_の描画
	void PresentAll();

	/// @brief 新しいwindowを生成
	/// @param _windowName windowの名前
	/// @param _windowSize windowのサイズ
	/// @param _windowType windowの種類
	/// @return 生成したwindowのポインタ
	Window* GenerateWindow(const std::wstring& _windowName, const Vec2& _windowSize, WindowType _windowType = WindowType::Sub);

	/// @brief game windowを非表示
	/// @param _windowPtr 隠したいwindowのポインタ
	void HideGameWindow(Window* _windowPtr) { ShowWindow(_windowPtr->GetHwnd(), SW_HIDE); }

	/// @brief game windowを表示
	/// @param _windowPtr 表示したいwindowのポインタ
	void ShowGameWindow(Window* _windowPtr) { ShowWindow(_windowPtr->GetHwnd(), SW_SHOW); }

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief  game windowの生成
	/// @param _title windowのタイトル
	/// @param _size  windowのサイズ
	/// @param _windowStyle windowのスタイル
	/// @param _windowPtr windowのポインタ
	/// @param _windowType windowの種類
	void CreateGameWindow(const wchar_t* _title, const Vec2& _size, UINT _windowStyle, Window* _windowPtr, WindowType _windowType);

	/// @brief main windowの更新
	void UpdateMainWindow();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<Window>> windows_;
	Window*                              pMainWindow_ = nullptr;

	class DxManager*                     dxManager_ = nullptr;

	bool                                 isProcessEnd_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// main windowを取得
	/// </summary>
	/// <returns></returns>
	Window* GetMainWindow() const { return pMainWindow_; }

	/// @brief 現在のアクティブなwindowを取得
	/// @return アクティブなwindowのポインタ
	Window* GetActiveWindow() const;

private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	WindowManager(const WindowManager&)            = delete;
	WindowManager(WindowManager&&)                 = delete;
	WindowManager& operator=(const WindowManager&) = delete;
	WindowManager& operator=(WindowManager&&)      = delete;
};

