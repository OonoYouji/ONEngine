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

    /// @brief プリファブをロードしてエンティティを作成
    /// @param path ファイルパス
    /// @param registry ロード先のレジストリ
    /// @return 作成されたエンティティのID。失敗時はkNullEntity
    static Engine::ECS::Entity InstantiatePrefab(const std::string& path, Engine::ECS::Registry& registry);
};

} // namespace Engine::Scene
