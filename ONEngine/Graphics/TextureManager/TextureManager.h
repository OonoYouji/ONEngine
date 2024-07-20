#pragma once

#include <unordered_map>
#include <string>

#include <Texture.h>



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
	/// テクスチャのゲッタ
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	const Texture& GetTexture(const std::string& name) const;

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="name"></param>
	/// <param name="filePath"></param>
	void Load(const std::string& name, const std::string& filePath);

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================


	static const std::string kDirectoryPath_;



	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, Texture> textures_;



private:
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager& operator= (TextureManager&&) = delete;
};