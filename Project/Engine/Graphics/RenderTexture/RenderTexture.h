#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Graphics/Resource/ResourceData/Texture.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// render texture
/// ////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTexture {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	RenderTexture() = default;
	~RenderTexture() = default;

	void Initialize(const Vector4& _clearColor, class DxManager* _dxManager);

	/// @brief render targetとして設定
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void SetRenderTarget(class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	std::unique_ptr<Texture> texture_; /// 書き込み先のテクスチャ
	DxResource renderTextureResource_;
	
	Vector4 clearColor_;

};

