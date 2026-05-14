#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Asset/Material.h"

namespace Engine::Graphics {
    class RenderDevice;
}

namespace Engine::Asset {

///
/// マテリアルアセットを管理するクラス
///
class MaterialManager {
public:
    static MaterialManager& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new MaterialManager();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief マテリアルをロードし、インデックスを返す
    int32_t LoadMaterial(const std::string& filePath);
    int32_t LoadMaterial(uint64_t guid);

    /// @brief インデックスからマテリアルを取得
    Material* GetMaterial(uint64_t guid);
    Material* GetMaterialByIndex(uint32_t index);

    std::shared_ptr<Material> LoadMaterialAsAsset(uint64_t guid);
    std::shared_ptr<Material> LoadMaterialAsAsset(const std::string& path);

private:
    MaterialManager();
    ~MaterialManager();

    static MaterialManager* instance_;

    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<uint64_t, std::shared_ptr<Material>> materials_;
    std::vector<std::shared_ptr<Material>> indexedMaterials_;
};

} // namespace Engine::Asset
