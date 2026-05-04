#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "Material.h"
#include "AssetHandle.h"

namespace Engine::Graphics {
	class RenderDevice;
}

namespace Engine::Asset {

///
/// マテリアルを管理するクラス
///
class MaterialManager {
public:
	static MaterialManager& GetInstance() {
		static MaterialManager instance;
		return instance;
	}

	void Initialize(Graphics::RenderDevice* device);
	void Shutdown();

	/// @brief マテリアルファイルをロード (.mat / JSON)
	/// @param filePath ファイルパス
	/// @return ロードされたマテリアルのインデックス。失敗した場合は -1。
	int32_t LoadMaterial(const std::string& filePath);

	/// @brief AssetRegistry経由でのロード
	std::shared_ptr<Material> LoadMaterialAsAsset(const std::string& pathOrGuid);

	/// @brief マテリアルを取得
	Material* GetMaterial(const std::string& name);
    Material* GetMaterialByIndex(uint32_t index);

private:
	MaterialManager();
	~MaterialManager();

private:
	Graphics::RenderDevice* device_ = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
    std::vector<std::shared_ptr<Material>> indexedMaterials_;
};

} // namespace Engine::Asset
