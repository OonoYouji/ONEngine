#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <array>

#include <PipelineState.h>
#include <Vector2.h>
#include <RenderTexture.h>

class RenderTexture;

using namespace Microsoft::WRL;


enum USE_EFFECT {
	LUMINANCE_EXTRACTION,	/// 輝度抽出
	BLOOM,					/// ブルーム
	BLUR,					/// ブラー
};


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
		ONE::DxDescriptor* dxDescriptor,
		uint32_t renderTargetLayerNumber
	);
	static void StaticFinalize();

	void ImGuiDebug(const std::string& treeNodeName);

	void Initialize();

	void ApplyBloom(
		RenderTexture* sourceRenderTexture
	);

	RenderTexture* GetBloomRenderTexture() {
		return renderTextures_[BLOOM].get();
	}

private:

	static std::unique_ptr<PipelineState> sBloom_;
	static std::unique_ptr<PipelineState> sBluer_;
	static std::unique_ptr<PipelineState> sLuminance_;


	std::array<std::unique_ptr<RenderTexture>, 3> renderTextures_;

	/// ブルーム用
	ComPtr<ID3D12Resource> bloomBuffer_;
	BloomData* bloomMappingData_ = nullptr;
	/// ブラー用
	ComPtr<ID3D12Resource> blurBuffer_;
	BlurData* blurMappingData_ = nullptr;
	/// 輝度抽出用
	ComPtr<ID3D12Resource> luminanceBuffer_;
	LuminanceData* luminanceMappingData_ = nullptr;

	int layerId_;	/// scene layer idの番号
	int layerNum_;	/// back, object3d, frontのどれか
};