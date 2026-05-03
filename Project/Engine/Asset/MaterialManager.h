#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "Material.h"
#include "AssetHandle.h"

namespace Engine::Graphics {

class RenderDevice;

///
/// マテリアルを管理するクラス
///
class MaterialManager {
public:
	static MaterialManager& GetInstance() {
		static MaterialManager instance;
		return instance;
	}

	void Initialize(RenderDevice* device);
	void Shutdown();

	/// @brief マテリアルファイルをロード (.mat / JSON)
	/// @param filePath ファイルパス
	/// @return ロードされたマテリアル名。失敗した場合は空文字。
	std::string LoadMaterial(const std::string& filePath);

	/// @brief AssetRegistry経由でのロード
	std::shared_ptr<Material> LoadMaterialAsAsset(const std::string& pathOrGuid);

	/// @brief マテリアルを取得
	Material* GetMaterial(const std::string& name);

private:
	MaterialManager();
	~MaterialManager();

private:
	RenderDevice* device_ = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
};

} // namespace Engine::Graphics
