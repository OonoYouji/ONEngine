#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Graphics/Resource/ResourceData/Texture.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ///////////////////////////////////////////////////
/// render texture
/// ///////////////////////////////////////////////////
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

	void Initialize(DXGI_FORMAT _format, const Vector4& _clearColor, const std::string& _name, class DxManager* _dxManager, class GraphicsResourceCollection* _resourceCollection);

	/// @brief render targetとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void SetRenderTarget(class DxCommand* _dxCommand, class DxDSVHeap* _dxDSVHeap);

	/// @brief 複数のrender targetとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	/// @param _dxDSVHeap DxDSVHeapのインスタンスへのポインタ
	/// @param _other 他のrender textureのvector
	void SetRenderTarget(class DxCommand* _dxCommand, class DxDSVHeap* _dxDSVHeap, const std::vector<std::unique_ptr<class RenderTexture>>& _other);

	/// @brief render textureとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void CreateBarrierRenderTarget(class DxCommand* _dxCommand);

	/// @brief srvとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void CreateBarrierPixelShaderResource(class DxCommand* _dxCommand);

	/// @brief RenderTextureの名前を取得
	/// @return std::string RenderTextureの名前
	const std::string& GetName() const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	std::string name_;
	Vector4 clearColor_;

	Handle rtvHandle_;
	Texture* texture_ = nullptr;


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
	/// @param _dxManager DxManagerへのポインタ
	/// @param _resourceCollection GraphicsResourceCollectionへのポインタ
	void Initialize(const std::string& _textureName, class DxManager* _dxManager, class GraphicsResourceCollection* _resourceCollection);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Texture* texture_ = nullptr;

};