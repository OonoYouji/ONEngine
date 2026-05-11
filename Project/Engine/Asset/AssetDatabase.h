#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

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

    /// @brief 指定ディレクトリをスキャンしてアセット情報を収集
    void Scan(const std::string& directory);

    /// @brief GUIDからファイルパスを取得
    std::string GetPathFromGuid(const std::string& guid);

    /// @brief ファイルパスからGUIDを取得
    std::string GetGuidFromPath(const std::string& path);

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    static AssetDatabase* instance_;

    std::unordered_map<std::string, std::string> guidToPath_;
    std::unordered_map<std::string, std::string> pathToGuid_;
};

} // namespace Engine::Asset
