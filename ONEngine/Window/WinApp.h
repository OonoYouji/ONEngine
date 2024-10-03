#pragma once


#include <Windows.h>

#include <memory>

#include <RenderTexture.h>
#include <DxDoubleBuffer.h>


namespace ONE {

	/// ===================================================
	/// window管理クラス
	/// ===================================================
	class WinApp final {
	public:

		/// ===================================================
		/// public : static objects
		/// ===================================================

		static const int kWindowSizeX = 1280;
		static const int kWindowSizeY = 720;

		/// ===================================================
		/// public : methods
		/// ===================================================

		WinApp() = default;
		~WinApp() = default;

		void Initialize(
			const wchar_t* windowName,
			WinApp* parent
		);

		void Finalize();

		UINT ProcessMessage();

		void PreDraw();
		void PostDraw(RenderTexture* renderTexture);

		void Present();



		void SetIsFullScreen(bool isFullScreen);
		bool GetIsFullScreen() const { return isFullScreen_; }

		const HWND& GetHWND() const { return hwnd_; }

		const WNDCLASS& GetWNDCLASS() const { return wc_; }

		const RECT& GetFullScreenRect() const { return fullscreenRect_; }
		const RECT& GetRect() const { return wrc_; }

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		void CreateGameWindow(
			WinApp* parent,
			const wchar_t* title, UINT windowStyle, int sizeX, int sizeY
		);

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

		RECT fullscreenRect_{  };
		bool isFullScreen_ = false;

		std::unique_ptr<DxDoubleBuffer> doubleBuffer_ = nullptr;

	private:
		WinApp(const WinApp&) = delete;
		WinApp(WinApp&&) = delete;
		WinApp& operator=(const WinApp&) = delete;
		WinApp& operator=(WinApp&&) = delete;
	};

}
