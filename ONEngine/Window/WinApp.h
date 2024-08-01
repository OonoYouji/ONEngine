#pragma once


#include <Windows.h>

namespace ONE {


	/// ===================================================
	/// window管理クラス
	/// ===================================================
	class WinApp final {
		WinApp() = default;
		~WinApp() = default;
	public:
		/// ===================================================
		/// public : static objects
		/// ===================================================

		static const int kWindowSizeX = 1280;
		static const int kWindowSizeY = 720;

	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		static WinApp* GetInstance();

		void Initialize();

		void Finalize();

		UINT ProcessMessage();



		const HWND& GetHWND() const { return hwnd_; }

		const WNDCLASS& GetWNDCLASS() const { return wc_; }

	private:
		
		/// ===================================================
		/// private : methods
		/// ===================================================

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		void CreateGameWindow(const wchar_t* title, UINT windowStyle, int sizeX, int sizeY);

		void TerminateGameWindow();

	private:

		/// ===================================================
		/// private : object
		/// ===================================================

		WNDCLASS wc_;
		RECT wrc_;
		HWND hwnd_;
		MSG msg_;
		UINT windowStyle_;

	private:
		WinApp(const WinApp&) = delete;
		WinApp(WinApp&&) = delete;
		WinApp& operator=(const WinApp&) = delete;
		WinApp& operator=(WinApp&&) = delete;
	};

}
