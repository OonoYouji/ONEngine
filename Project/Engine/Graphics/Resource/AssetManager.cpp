#include "AssetManager.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"

namespace Engine {

void AssetManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;
}

void AssetManager::Shutdown() {
    models_.clear();
}

void AssetManager::LoadModel(const std::string& name, const std::string& filePath) {
    if (models_.count(name)) return;
    
    auto meshes = Graphics::ModelLoader::LoadModel(device_, filePath);
    if (!meshes.empty()) {
        models_[name] = std::move(meshes);
    }
}

const std::vector<std::unique_ptr<Graphics::Mesh>>& AssetManager::GetMeshes(const std::string& name) {
    static std::vector<std::unique_ptr<Graphics::Mesh>> empty;
    auto it = models_.find(name);
    return (it != models_.end()) ? it->second : empty;
}

} // namespace Engine
