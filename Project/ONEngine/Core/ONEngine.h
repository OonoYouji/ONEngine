#pragma once

#include <memory>
#include <unordered_map>

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxShaderCompiler.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDebug.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDepthStencil.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDoubleBuffer.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"

#include "WindowManager/WinApp.h"
#include "WindowManager/Console.h"


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

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		/// main loop
		bool isRunning_ = true;

		/// direct x 
		std::unique_ptr<ONE::DxCommon> dxCommon_ = nullptr;

		/// window 
		std::unordered_map<std::string, std::unique_ptr<ONE::WinApp>> winApps_;
		ONE::WinApp* mainWindow_   = nullptr;
		ONE::WinApp* activeWindow_ = nullptr;

		/// imgui console
		std::unique_ptr<Console> console_;
		
	};
}