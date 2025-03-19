#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Graphics/Resource/ResourceData/Texture.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// render texture
/// ////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTexture {
private:
	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct Handle {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	RenderTexture() = default;
	~RenderTexture() = default;

	void Initialize(DXGI_FORMAT _format, const Vector4& _clearColor, class DxManager* _dxManager, class GraphicsResourceCollection* _resourceCollection);

	/// @brief render targetとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void SetRenderTarget(class DxCommand* _dxCommand, class DxDSVHeap* _dxDSVHeap);

	/// @brief render textureの開始
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void Begin(class DxCommand* _dxCommand, class DxDSVHeap* _dxDSVHeap);

	/// @brief render textureの終了
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void End(class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	//std::unique_ptr<Texture> texture_; /// 書き込み先のテクスチャ
	Handle rtvHandle_;
	Texture* texture_ = nullptr;
	
	Vector4 clearColor_;

};

