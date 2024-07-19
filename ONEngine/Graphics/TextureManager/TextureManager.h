#pragma once



/// ===================================================
/// テクスチャの管理クラス
/// ===================================================
class TextureManager final {
	TextureManager() = default;
	~TextureManager() = default;
public:

private:
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager& operator= (TextureManager&&) = delete;
};