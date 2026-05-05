#pragma once

#include <vector>
#include <string>
#include <map>
#include "Engine/Core/Math/Math.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Shader/PipelineState.h"
#include "Schema/Schema.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// 描画コンテキスト
///
struct RenderContext {
	ID3D12GraphicsCommandList* commandList;
	D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress;
	D3D12_GPU_VIRTUAL_ADDRESS pointLightBufferAddress;
	uint32_t frameIndex;
	DXGI_FORMAT rtvFormat; // 追加: 現在の描画ターゲットの形式
};

///
/// 描画リクエスト
///
struct RenderRequest {
	uint32_t modelIndex;
	uint32_t materialIndex;
	uint32_t vertexOffset; // 追加
	Engine::Math::Matrix4x4 world;
};

///
/// 描画を一括管理するクラス
///
class Renderer {
public:
	static Renderer& GetInstance() {
		static Renderer instance;
		return instance;
	}

	void Initialize(RenderDevice* device);
	void Shutdown();

	/// @brief 描画リクエストの追加
	void PushRequest(const RenderRequest& request);

	/// @brief データの抽出（ロジックの後に呼ばれる）
	void Extract();

	/// @brief Z-Prepassの実行
	void RenderZPrepass(const RenderContext& context);

	/// @brief メイン描画の実行
	void Render(const RenderContext& context);

	void ClearQueue();

private:
	void RenderInternal(const RenderContext& context, const PipelineStateDesc& baseDesc);

	RenderDevice* device_ = nullptr;
	std::vector<RenderRequest> queue_;
	
	// インスタンスデータ用バッファ（三重バッファリング）
	static constexpr uint32_t kBufferCount = 3;
	std::unique_ptr<StructuredBuffer> instanceSBs_[kBufferCount];
	const uint32_t kMaxInstances = 2048;
};

} // namespace Engine::Graphics
