#pragma once

#include <vector>
#include <string>
#include <map>
#include "Engine/Core/Math/Math.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Schema/Schema.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// 描画リクエスト
///
struct RenderRequest {
	std::string modelName;
	std::string materialName;
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

	/// @brief 描画実行（フレームの最後に一括で呼ばれる）
	void Render(ID3D12GraphicsCommandList* commandList, const D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress);

	void ClearQueue();

private:
	RenderDevice* device_ = nullptr;
	std::vector<RenderRequest> queue_;
	
	// インスタンスデータ用バッファ（二重バッファリング）
	static constexpr uint32_t kBufferCount = 3;
	std::unique_ptr<StructuredBuffer> instanceSBs_[kBufferCount];
	const uint32_t kMaxInstances = 2048;
};

} // namespace Engine::Graphics
