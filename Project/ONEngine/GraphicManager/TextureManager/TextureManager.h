#pragma once

/// std
#include <memory>
#include <unordered_map>
#include <string>

/// engine
#include "Texture.h"



/// ===================================================
/// テクスチャの管理クラス
/// ===================================================
class TextureManager final {
	TextureManager() = default;
	~TextureManager() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================


	/// <summary>
	/// インスタンス確保
	/// </summary>
	static TextureManager* GetInstance();


	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	/// <summary>
	/// テクスチャのゲッタ
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	Texture* GetTexture(const std::string& name) const;

	const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextureAll() const {
		return textures_;
	}

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="name"></param>
	/// <param name="filePath"></param>
	void Load(const std::string& texName, const std::string& filePath);


	Texture* CreateUAVTexture(
		const std::string& _textureName,
		const Vec2& _textureSize,
		DXGI_FORMAT _format
	);

	void AddTexture(
		const std::string& name,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
	);

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================


	static const std::string kDirectoryPath_;



	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;



private:
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager& operator= (TextureManager&&) = delete;
};