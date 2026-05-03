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
            TextureManager::GetInstance().LoadTexture(material->textureName);
        }

        // パラメータの読み込み
        if (data.contains("parameters")) {
            auto& params = data["parameters"];
            Engine::Console::Log(std::format("Material [{}]: 'parameters' found.", material->name));
            if (params.contains("baseColor")) {
                auto& c = params["baseColor"];
                if (c.is_array() && c.size() >= 3) {
                    material->baseColor.x = c[0].get<float>();
                    material->baseColor.y = c[1].get<float>();
                    material->baseColor.z = c[2].get<float>();
                    material->baseColor.w = (c.size() > 3) ? c[3].get<float>() : 1.0f;
                    Engine::Console::Log(std::format("  baseColor loaded: ({}, {}, {}, {})", 
                        material->baseColor.x, material->baseColor.y, material->baseColor.z, material->baseColor.w));
                }
            }
        } else {
            Engine::Console::LogWarning(std::format("Material [{}]: 'parameters' NOT found. Using defaults.", material->name));
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
