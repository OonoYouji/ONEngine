#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <string>

#include <Vector4.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>


using namespace Microsoft::WRL;

/// ===================================================
/// マテリアル
/// ===================================================
class Material final {
public:

	struct MaterialData final {
		Vec4 color;
		Mat4 uvTransform;
		int isLighting = true;
	};

public:

	Material();
	~Material();

	/// ===================================================
	/// public : methods
	/// ===================================================


	void CreateMaterial(const std::string& textureName);

	void SetColor(const Vector4& color);
	void SetIsLighting(bool isLighting);


	/// <summary>
	/// バッファの生成
	/// </summary>
	void CreateBuffer();



	/// <summary>
	/// テクスチャのファイルパスをセット
	/// </summary>
	/// <param name="filePath"> : TextureManager::Load();の引数と同じ</param>
	void SetFilePath(const std::string& filePath);

	void SetTextureName(const std::string& textureName);
	

	/// <summary>
	/// commandListにテクスチャを積む
	/// </summary>
	/// <param name="commandList"> : コマンドリスト</param>
	/// <param name="rootParamIndex"> : ルートパラメータのインデックス</param>
	void BindTexture(ID3D12GraphicsCommandList* commandList, uint32_t rootParamIndex);

	/// <summary>
	/// commandListにマテリアルを積む
	/// </summary>
	/// <param name="commandList"> : コマンドリスト</param>
	/// <param name="rootParamIndex"> : ルートパラメータのインデックス</param>
	void BindMaterial(ID3D12GraphicsCommandList* commandList, uint32_t rootParamIndex);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string filePath_;
	std::string texName_;

	ComPtr<ID3D12Resource> materialBuffer_;
	MaterialData* materialData_ = nullptr;

};