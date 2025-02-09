#pragma once

/// std
#include <string>
#include <vector>
#include <memory>

/// engine
#include "../ResourceData/Mesh.h"

/// ===================================================
/// グラフィクスリソースのローダー
/// ===================================================
class GraphicsResourceLoader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsResourceLoader(class DxManager* _dxManager);
	~GraphicsResourceLoader();

	void Initialize();

	/// @brief textureの読み込み (.png, .jpg)
	/// @param _filePath .slnファイルからの相対パス
	void LoadTexture(const std::string& _filePath);

	/// @brief modelの読み込み (.obj)
	/// @param _filePath .slnファイルからの相対パス
	size_t LoadModelObj(const std::string& _filePath);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* dxManager_;

	unsigned int assimpLoadFlags_; /// assimpで.objを読み込むときに使用するフラグ


	/// mesh管理
	std::vector<std::unique_ptr<Mesh>> meshes_;


};

