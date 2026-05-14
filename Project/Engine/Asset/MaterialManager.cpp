#include "MaterialManager.h"
#include "TextureManager.h"
#include "AssetDatabase.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

namespace Engine::Asset {

MaterialManager* MaterialManager::instance_ = nullptr;

MaterialManager::MaterialManager() = default;
MaterialManager::~MaterialManager() {
    Shutdown();
}

void MaterialManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;

    // AssetRegistryへの登録
    AssetRegistry::GetInstance().RegisterType<Material>(AssetType::Material);
    AssetRegistry::GetInstance().RegisterLoader(AssetType::Material, [this](const std::string& pathOrGuid) {
        if (!pathOrGuid.empty() && std::all_of(pathOrGuid.begin(), pathOrGuid.end(), ::isdigit)) {
            return this->LoadMaterialAsAsset(std::stoull(pathOrGuid));
        }
        return this->LoadMaterialAsAsset(pathOrGuid);
    });
}

void MaterialManager::Shutdown() {
    materials_.clear();
    indexedMaterials_.clear();
}

int32_t MaterialManager::LoadMaterial(const std::string& pathOrGuid) {
    std::shared_ptr<Material> mat;
    if (!pathOrGuid.empty() && std::all_of(pathOrGuid.begin(), pathOrGuid.end(), ::isdigit)) {
        mat = LoadMaterialAsAsset(std::stoull(pathOrGuid));
    } else {
        mat = LoadMaterialAsAsset(pathOrGuid);
    }

    if (!mat) return -1;

    // 既に登録済みかチェック
    for (size_t i = 0; i < indexedMaterials_.size(); ++i) {
        if (indexedMaterials_[i]->GetGuid() == mat->GetGuid()) return static_cast<int32_t>(i);
    }

    indexedMaterials_.push_back(mat);
    return static_cast<int32_t>(indexedMaterials_.size() - 1);
}

int32_t MaterialManager::LoadMaterial(uint64_t guid) {
    auto mat = LoadMaterialAsAsset(guid);
    if (!mat) return -1;

    for (size_t i = 0; i < indexedMaterials_.size(); ++i) {
        if (indexedMaterials_[i]->GetGuid() == mat->GetGuid()) return static_cast<int32_t>(i);
    }

    indexedMaterials_.push_back(mat);
    return static_cast<int32_t>(indexedMaterials_.size() - 1);
}

std::shared_ptr<Material> MaterialManager::LoadMaterialAsAsset(uint64_t guid) {
    if (guid == 0) return nullptr;
    if (materials_.count(guid)) return materials_[guid];

    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") return nullptr;

    std::ifstream file(path);
    if (!file.is_open()) return nullptr;

    try {
        json data = json::parse(file);
        auto material = std::make_shared<Material>();
        material->SetGuid(guid);
        material->SetPath(path);
        
        material->name = data.value("name", "");
        material->pipelineName = data.value("pipeline", "");
        
        std::string textureName = data.value("texture", "");
        if (!textureName.empty()) {
            material->textureIndex = TextureManager::GetInstance().LoadTexture(textureName);
        } else {
            material->textureIndex = 0xFFFFFFFF;
        }

        if (data.contains("parameters")) {
            auto& params = data["parameters"];
            if (params.contains("baseColor")) {
                auto& c = params["baseColor"];
                if (c.is_array() && c.size() >= 3) {
                    material->baseColor.x = c[0].get<float>();
                    material->baseColor.y = c[1].get<float>();
                    material->baseColor.z = c[2].get<float>();
                    material->baseColor.w = (c.size() > 3) ? c[3].get<float>() : 1.0f;
                }
            }
        }

        material->OnLoaded();
        materials_[guid] = material;
        return material;
    }
    catch (...) {
        return nullptr;
    }
}

std::shared_ptr<Material> MaterialManager::LoadMaterialAsAsset(const std::string& path) {
    uint64_t guid = AssetDatabase::GetInstance().GetGuidFromPath(path);
    if (guid != 0) return LoadMaterialAsAsset(guid);

    // GUIDがない場合（未登録アセット、またはメモリ内のみ）
    return nullptr;
}

Material* MaterialManager::GetMaterial(uint64_t guid) {
    auto it = materials_.find(guid);
    return (it != materials_.end()) ? it->second.get() : nullptr;
}

Material* MaterialManager::GetMaterialByIndex(uint32_t index) {
    if (index >= indexedMaterials_.size()) return nullptr;
    return indexedMaterials_[index].get();
}

} // namespace Engine::Asset
