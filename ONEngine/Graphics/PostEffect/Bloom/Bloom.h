#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>

#include <PipelineState.h>

class RenderTexture;

using namespace Microsoft::WRL;


class Bloom final {
public:

	struct BloomData final {
		float intensity; /// 強度
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

	static std::unique_ptr<PipelineState> sPipeline_;

	


};