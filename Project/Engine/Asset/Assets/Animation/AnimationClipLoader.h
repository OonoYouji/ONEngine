#pragma once

/// engine
#include "../IAssetLoader.h"
#include "AnimationClip.h"
#include "Engine/Asset/Meta/MetaFile.h"

namespace ONEngine::Asset {

/**
 * @class AssetLoader<AnimationClip>
 * @brief AnimationClipアセット専用のアセットローダーの特殊化クラス
 */
template<>
class AssetLoader<AnimationClip> : public IAssetLoader {
public:
    /**
     * @brief デフォルトコンストラクタ
     */
    AssetLoader() = default;

    /**
     * @brief デストラクタ
     */
    ~AssetLoader() override = default;

    /**
     * @brief ディスクからアニメーションクリップ（.anim）ファイルをJSONとしてロードします。
     * @param _filepath ロード対象のファイルパス
     * @param meta アニメーションクリップのメタデータ
     * @return ロードされたAnimationClipアセット（失敗時はstd::nullopt）
     */
    [[nodiscard]]
    std::optional<AnimationClip> Load(const std::string& _filepath, typename Meta<AnimationClip::MetaData> meta);

    /**
     * @brief 既存のアニメーションクリップに対して再ロード（リロード）を実行します。
     * @param _filepath 再ロード対象のファイルパス
     * @param _src 再ロード元のAnimationClipオブジェクトへのポインタ
     * @param meta アニメーションクリップのメタデータ
     * @return 再ロードされたAnimationClipアセット（失敗時はstd::nullopt）
     */
    std::optional<AnimationClip> Reload(const std::string& _filepath, AnimationClip* _src, typename Meta<AnimationClip::MetaData> meta);

    /**
     * @brief アニメーションクリップに対応するメタデータを取得します。
     * @param _filepath 対象アセットファイルのパス
     * @return 解析・構築されたメタデータオブジェクト
     */
    Meta<typename AnimationClip::MetaData> GetMetaData(const std::string& _filepath);
};

} /// namespace ONEngine::Asset
