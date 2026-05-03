#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

namespace Engine {

///
/// アセットのパスとGUIDの対応を管理するデータベース
///
class AssetDatabase {
public:
    static AssetDatabase& GetInstance() {
        static AssetDatabase instance;
        return instance;
    }

    /// @brief 指定したルートディレクトリ以下をスキャンしてデータベースを構築
    void Scan(const std::string& rootDir);

    /// @brief パスからGUIDを取得
    std::string GetGuidFromPath(const std::string& path) const;

    /// @brief GUIDからパスを取得
    std::string GetPathFromGuid(const std::string& guid) const;

    /// @brief 全ての登録済みアセットをデバッグ出力
    void Dump() const;

private:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

private:
    std::unordered_map<std::string, std::string> pathToGuid_;
    std::unordered_map<std::string, std::string> guidToPath_;
};

} // namespace Engine
