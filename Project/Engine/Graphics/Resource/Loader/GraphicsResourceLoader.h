#pragma once

/// std
#include <string>


/// ===================================================
/// グラフィクスリソースのローダー
/// ===================================================
class GraphicsResourceLoader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsResourceLoader();
	~GraphicsResourceLoader();

	void Initialize();

	/// @brief textureの読み込み (.png, .jpg)
	/// @param _filePath .slnファイルからの相対パス
	void LoadTexture(const std::string& _filePath);

	/// @brief modelの読み込み (.obj)
	/// @param _filePath .slnファイルからの相対パス
	void LoadModelObj(const std::string& _filePath);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
};

