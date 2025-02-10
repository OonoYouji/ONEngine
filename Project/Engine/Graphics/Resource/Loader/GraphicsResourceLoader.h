#pragma once

/// std
#include <string>
#include <vector>
#include <memory>

/// ===================================================
/// グラフィクスリソースのローダー
/// ===================================================
class GraphicsResourceLoader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsResourceLoader(class DxManager* _dxManager, class GraphicsResourceCollection* _collection);
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

	class DxManager*                  dxManager_;
	class GraphicsResourceCollection* resourceCollection_;

	unsigned int assimpLoadFlags_; /// assimpで.objを読み込むときに使用するフラグ


};

