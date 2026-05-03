#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace Engine::Graphics {
    class Mesh;
    class RenderDevice;
}

namespace Engine {

///
/// アセット全体を統合管理するクラス
///
class AssetManager {
public:
    static AssetManager& GetInstance() {
        static AssetManager instance;
        return instance;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief モデルをロードしてキャッシュ
    void LoadModel(const std::string& name, const std::string& filePath);
    
    /// @brief キャッシュされたメッシュリストを取得
    const std::vector<std::unique_ptr<Graphics::Mesh>>& GetMeshes(const std::string& name);

private:
    AssetManager() = default;
    ~AssetManager() = default;

private:
    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<std::string, std::vector<std::unique_ptr<Graphics::Mesh>>> models_;
};

} // namespace Engine
