#pragma once

/// engine
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace ONEngine {
class DxManager;
class DxDepthStencil;
class DxCommand;
class DxDSVHeap;
class RenderTexture;
}

namespace ONEngine::Asset {
class AssetCollection;
}



/// ///////////////////////////////////////////////////
/// render texture
/// ///////////////////////////////////////////////////
namespace ONEngine {

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

	RenderTexture();
	~RenderTexture();

	void Initialize(DXGI_FORMAT _format, const Vector4& _clearColor, const Vector2& _textureSize, const std::string& _name, DxManager* _dxm, DxDepthStencil* _dxDepthStencil, Asset::AssetCollection* _assetCollection);

	/// @brief render targetとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap);

	/// @brief 複数のrender targetとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	/// @param _dxDSVHeap DxDSVHeapのインスタンスへのポインタ
	/// @param _other 他のrender textureのvector
	void SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap, const std::vector<std::unique_ptr<RenderTexture>>& _other);

	/// @brief render textureとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void CreateBarrierRenderTarget(DxCommand* _dxCommand);

	/// @brief srvとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void CreateBarrierPixelShaderResource(DxCommand* _dxCommand);

	/// @brief RenderTextureの名前を取得
	/// @return std::string RenderTextureの名前
	const std::string& GetName() const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	DxDepthStencil* pDxDepthStencil_ = nullptr;

	std::string name_;
	Vector4 clearColor_;

	Handle rtvHandle_;
	Asset::Texture* texture_ = nullptr;


};



/// ///////////////////////////////////////////////////
/// UAVTexture
/// ///////////////////////////////////////////////////
class UAVTexture {
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

	UAVTexture();
	~UAVTexture();

	/// @brief uav textureの初期化
	/// @param _textureName textureの名前
	/// @param _dxm DxManagerへのポインタ
	/// @param _assetCollection AssetCollectionへのポインタ
	void Initialize(const std::string& _textureName, DxManager* _dxm, class Asset::AssetCollection* _assetCollection);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Asset::Texture* texture_ = nullptr;

};

} /// ONEngine
