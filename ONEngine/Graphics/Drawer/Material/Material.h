#pragma once

#include <string>



/// ===================================================
/// マテリアル
/// ===================================================
class Material final {
public:

	Material();
	~Material();

	/// ===================================================
	/// public : methods
	/// ===================================================


	/// <summary>
	/// テクスチャのファイルパスをセット
	/// </summary>
	/// <param name="filePath"> : TextureManager::Load();の引数と同じ</param>
	void SetTextureName(const std::string& filePath);



private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string filePath_;
	
	///- uvTransform
};