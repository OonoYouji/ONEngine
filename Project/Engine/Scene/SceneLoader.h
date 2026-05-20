#pragma once

#include <string>
#include <vector>
#include "Engine/ECS/Registry.h"

namespace Engine::Scene {

///
/// シーンやプリファブをJSONからロードするクラス
///
class SceneLoader {
public:
    /// @brief シーンファイルをロード
    /// @param path ファイルパス
    /// @param registry ロード先のレジストリ
    /// @return 成功したらtrue
    static bool LoadScene(const std::string& path, Engine::ECS::Registry& registry);

    /// @brief シーンファイルを保存
    /// @param path ファイルパス
    /// @param registry 保存元のレジストリ
    /// @return 成功したらtrue
    static bool SaveScene(const std::string& path, Engine::ECS::Registry& registry);

    /// @brief プリファブをロードしてエンティティを作成
    /// @param path ファイルパス
    /// @param registry ロード先のレジストリ
    /// @return 作成されたエンティティのID。失敗時はkNullEntity
    static Engine::ECS::Entity InstantiatePrefab(const std::string& path, Engine::ECS::Registry& registry);

    /// @brief プリファブとしてエンティティを保存
    /// @param path 保存先パス
    /// @param entity 保存対象のエンティティ
    /// @param registry 所属レジストリ
    /// @return 成功したらtrue
    static bool SavePrefab(const std::string& path, Engine::ECS::Entity entity, Engine::ECS::Registry& registry);
};

} // namespace Engine::Scene
