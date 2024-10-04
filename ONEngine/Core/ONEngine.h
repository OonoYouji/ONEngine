#pragma once

#include <memory>
#include <unordered_map>

#include <DxCommon.h>

#include <DxShaderCompiler.h>
#include <DxDebug.h>
#include <DxDepthStencil.h>
#include <DxDoubleBuffer.h>
#include <DxDescriptor.h>
#include <DxCommand.h>
#include <DxDevice.h>

#include <WinApp.h>
#include <Console.h>


/// ===================================================
/// エンジン
/// ===================================================
class ONEngine final {
	ONEngine() = default;
	~ONEngine() = default;
public:

	static void Initialize();
	static void Finalize();

	static void Update();

	static ONE::DxCommon* GetDxCommon();
	static ONE::WinApp*   GetMainWinApp();
	static ONE::WinApp*   GetActiveWinApp();
	static const std::unordered_map<std::string, std::unique_ptr<ONE::WinApp>>& GetWinApps();

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

		void Initialize();
		void Finalize();

		void Update();

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

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