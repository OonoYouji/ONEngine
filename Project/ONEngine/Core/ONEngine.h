#pragma once

#include <memory>
#include <unordered_map>

#include "WindowManager/WinApp.h"
#include "WindowManager/Console.h"

/// ===================================================
/// 前方宣言
/// ===================================================
class Time;
class Input;
class ImGuiManager;
class SceneManager;

namespace ONE {
	class DxCommon;
	class DxShaderCompiler;
	class DxDebug;
	class DxDepthStencil;
	class DxDoubleBuffer;
	class DxDescriptor;
	class DxCommand;
	class DxDevice;
	class DxDevice;
}


/// ===================================================
/// エンジン
/// ===================================================
class ONEngine final {
	ONEngine() = default;
	~ONEngine() = default;
public:

	static void Initialize(
		const wchar_t* windowName = L"game window",
		bool isCreateGameWindow = true
	);
	static void Finalize();

	static void Update();

	static void PreDraw();
	static void PostDraw();

	static ONE::DxCommon* GetDxCommon();
	static ONE::WinApp*   GetMainWinApp();
	static ONE::WinApp*   GetActiveWinApp();
	static const std::unordered_map<std::string, std::unique_ptr<ONE::WinApp>>& GetWinApps();


	static bool IsRunning();


private:
	ONEngine(const ONEngine&)            = delete;
	ONEngine(ONEngine&&)                 = delete;
	ONEngine& operator=(const ONEngine&) = delete;
	ONEngine& operator=(ONEngine&&)      = delete;
};


/// ===================================================
/// エンジンの機能
/// ===================================================
namespace {
	class System final {
		friend class ONEngine;
	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		void Initialize(const wchar_t* windowName, bool isCreateGameWindow);
		void Finalize();

		void Update();

		void PreDraw();
		void PostDraw();

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		/// main loop
		bool isRunning_ = true;

		/// window 
		std::unordered_map<std::string, std::unique_ptr<ONE::WinApp>> winApps_;
		ONE::WinApp* mainWindow_   = nullptr;
		ONE::WinApp* activeWindow_ = nullptr;


		/// engine...
		std::unique_ptr<ONE::DxCommon> dxCommon_ = nullptr; /// graphics engine
		Input* input_ = nullptr;                            /// input... keyboard, mouse,
		Time*  time_  = nullptr;


		/// gane...
		SceneManager* sceneManager_ = nullptr;

		
#ifdef _DEBUG /// release not building objects
		std::unique_ptr<Console> console_  = nullptr; /// console window
		ImGuiManager*            imguiManager_ = nullptr;
#endif // _DEBUG


		
	};
}