#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"

#include <cmath>



namespace ONE {
	class WinApp;
	class DxCommon;
}

using namespace Microsoft::WRL;

class ImGuiManager final {
private:

	ImGuiManager() = default;
	~ImGuiManager() = default;

	ONE::DxCommon* dxCommon_;

public:

	static ImGuiManager* GetInstance();

	void Initialize(ONE::WinApp* winApp, ONE::DxCommon* dxCommon);
	void Finalize();

	void BeginFrame();
	void EndFrame();

private:

	void StyleSetting();
	ImVec4 ColorToVec4(uint32_t color);

};

