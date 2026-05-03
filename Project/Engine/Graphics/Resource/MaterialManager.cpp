#include "MaterialManager.h"
#include "TextureManager.h"
#include "AssetDatabase.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

namespace Engine::Graphics {

MaterialManager::MaterialManager() = default;
MaterialManager::~MaterialManager() = default; // 前方宣言対応

void MaterialManager::Initialize(RenderDevice* device) {
    device_ = device;
}

void MaterialManager::Shutdown() {
    materials_.clear();
}

std::string MaterialManager::LoadMaterial(const std::string& pathOrGuid) {
    // GUID変換
    std::string guid = pathOrGuid;
    if (AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid) == "") {
        std::string found = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
        if (found != "") guid = found;
    }

    if (materials_.count(guid)) return guid;

    // 実際のパス取得
    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") path = pathOrGuid;

    std::ifstream file(path);
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("Failed to open material: {}", path));
        return "";
    }

    try {
        json data = json::parse(file);
        auto material = std::make_unique<Material>();
        
        material->name = data.value("name", "");
        material->pipelineName = data.value("pipeline", "");
        
        // テクスチャの依存解決
        material->textureName = data.value("texture", "");
        if (!material->textureName.empty()) {
            // 自動ロード！
            TextureManager::GetInstance().LoadTexture(material->textureName);
        }

        materials_[guid] = std::move(material);
        Engine::Console::Log(std::format("MaterialManager: Loaded [{}]", path));
        return guid;
    }
    catch (const std::exception& e) {
        Engine::Console::LogError(std::format("Failed to parse material: {}\n{}", path, e.what()));
        return "";
    }
}

Material* MaterialManager::GetMaterial(const std::string& pathOrGuid) {
    std::string guid = pathOrGuid;
    if (AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid) == "") {
        std::string found = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
        if (found != "") guid = found;
    }

    auto it = materials_.find(guid);
    return (it != materials_.end()) ? it->second.get() : nullptr;
}

} // namespace Engine::Graphics
