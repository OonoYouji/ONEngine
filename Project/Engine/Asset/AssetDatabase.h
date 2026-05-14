#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <cstdint>

namespace Engine::Asset {

///
/// アセットのパスとGUIDの対応を管理するデータベース
///
class AssetDatabase {
public:
    static AssetDatabase& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new AssetDatabase();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    /// @brief 指定ディレクトリをスキャンしてアセット情報を収集。必要なら.metaを生成。
    void Scan(const std::string& directory);

    /// @brief GUIDからファイルパスを取得
    std::string GetPathFromGuid(uint64_t guid);

    /// @brief ファイルパスからGUIDを取得
    uint64_t GetGuidFromPath(const std::string& path);

    /// @brief 新しいGUIDを生成
    uint64_t GenerateGuid();

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    static AssetDatabase* instance_;

    std::unordered_map<uint64_t, std::string> guidToPath_;
    std::unordered_map<std::string, uint64_t> pathToGuid_;

    /// @brief .metaファイルを保存
    void SaveMeta(const std::filesystem::path& assetPath, uint64_t guid);
};

} // namespace Engine::Asset
