#include "MaterialManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"
#include "Externals/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

namespace Engine::Graphics {

void MaterialManager::Initialize(RenderDevice* device) {
    device_ = device;
}

void MaterialManager::Shutdown() {
    materials_.clear();
}

std::string MaterialManager::LoadMaterial(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("Failed to open material file: {}", filePath));
        return "";
    }

    try {
        json data = json::parse(file);
        auto material = std::make_unique<Material>();
        
        material->name = data.value("name", "");
        if (material->name.empty()) {
            material->name = std::filesystem::path(filePath).stem().string();
        }

        material->pipelineName = data.value("pipeline", "");
        material->textureName = data.value("texture", "");

        std::string name = material->name;
        materials_[name] = std::move(material);
        
        Engine::Console::Log(std::format("Loaded Material Asset: {}", name));
        return name;
    }
    catch (const std::exception& e) {
        Engine::Console::LogError(std::format("Failed to parse material file: {}\n{}", filePath, e.what()));
        return "";
    }
}

Material* MaterialManager::GetMaterial(const std::string& name) {
    auto it = materials_.find(name);
    return (it != materials_.end()) ? it->second.get() : nullptr;
}

} // namespace Engine::Graphics
