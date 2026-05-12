#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Asset/Mesh.h"
#include "Asset/Model.h"
#include "Asset/AssetHandle.h"

namespace Engine::Graphics {
    class RenderDevice;
    class StructuredBuffer;
}

namespace Engine::Asset {

///
/// アセット全体を管理するクラス
///
class AssetManager {
public:
    static AssetManager& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new AssetManager();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief ファイルパスまたはGUIDからモデルをアセットとしてロード
    std::shared_ptr<Model> LoadModelAsAsset(const std::string& pathOrGuid);

    /// @brief 旧方式の互換性用
    uint32_t LoadModel(const std::string& pathOrGuid);

    /// @brief インデックスからモデルを取得
    std::shared_ptr<Model> GetModelByIndex(uint32_t index);

    /// @brief ロード済みメッシュリストを取得
    const std::vector<std::unique_ptr<Mesh>>& GetMeshes(const std::string& pathOrGuid);
    const std::vector<std::unique_ptr<Mesh>>& GetMeshesByIndex(uint32_t index);

    D3D12_GPU_VIRTUAL_ADDRESS GetMeshInfoBufferAddress() const;
    void UpdateMeshInfoBuffer();

private:
    AssetManager() = default;
    ~AssetManager() = default;

    static AssetManager* instance_;

    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Model>> models_;
    std::vector<std::shared_ptr<Model>> indexedModels_;

    std::unique_ptr<Graphics::StructuredBuffer> meshInfoBuffer_;

    std::string ToGuid(const std::string& pathOrGuid);
};

} // namespace Engine::Asset
