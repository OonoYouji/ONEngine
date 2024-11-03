#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <array>

#include "GraphicManager/RenderTextureManager/RenderTexture.h"
#include "GraphicManager/PipelineState/PipelineState.h"
#include "Math/Vector2.h"

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
		int radius;
		float sigma;
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

	void ImGuiDebug(const std::string& treeNodeName);

	void Initialize();

	void SetTexSize(const Vec2& texSize);
	void SetIntensity(float intensity);
	void SetRadius(int radius);

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