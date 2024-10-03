#pragma once

#include <memory>

#include <DxCommon.h>

#include <DxShaderCompiler.h>
#include <DxDebug.h>
#include <DxDepthStencil.h>
#include <DxDoubleBuffer.h>
#include <DxDescriptor.h>
#include <DxCommand.h>
#include <DxDevice.h>

#include <WinApp.h>


/// ===================================================
/// エンジン
/// ===================================================
class ONEngine final {
	ONEngine() = default;
	~ONEngine() = default;
public:

	static void Initialize();
	static void Finalize();

	static ONE::DxCommon* GetDxCommon();
	static ONE::WinApp*   GetWinApp();

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


	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		std::unique_ptr<ONE::DxCommon> dxCommon_ = nullptr;
		std::unique_ptr<ONE::WinApp>   winApp_   = nullptr;

	};
}