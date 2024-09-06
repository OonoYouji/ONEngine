#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>

#include <PipelineState.h>
#include <Vector2.h>

class RenderTexture;

using namespace Microsoft::WRL;


class Bloom final {
public:

	struct BloomData final {
		float intensity; /// 強度
		Vec2 texSize; /// ピクセルの範囲
	};
	
	struct BlurData final {
		Vec2 texSize; /// ピクセルの範囲
	};
	
	struct LuminanceData final {
		float threshold; /// 閾値
	};

public:

	static void StaticInitialize(
		ID3D12GraphicsCommandList* commandList,
		uint32_t renderTargetLayerNumber
	);
	static void StaticFinalize();

	static void ImGuiDebug();

	static void CreateBloomRenderTexture(
		RenderTexture* sourceRenderTexture
	);

private:

	static std::unique_ptr<PipelineState> sBloom_;
	static std::unique_ptr<PipelineState> sBluer_;
	static std::unique_ptr<PipelineState> sLuminance_;

	


};