#pragma once

/// std
#include <string>
#include <vector>
#include <variant>

/// engine
#include "../IAsset.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace ONEngine::Asset {

/**
 * @struct AnimationKeyframe
 * @brief アニメーションの特定の時間におけるキーフレーム値と補間方法を表す構造体
 */
struct AnimationKeyframe {
    float time;                                           ///< キーフレームの時間（秒またはフレーム）
    std::variant<float, Vector2, Vector3, Vector4> value; ///< アニメーション値のデータ（float, Vector2, Vector3, Vector4のいずれか）
    std::string interpolation = "Linear";                 ///< 補間方法（"Linear", "Step", "Cubic" 等）
};

/**
 * @struct AnimationTrack
 * @brief 1つのコンポーネントの特定プロパティ変化を表すアニメーショントラック構造体
 */
struct AnimationTrack {
    std::string componentName;                ///< 対象コンポーネント名（"Transform" など）
    std::string propertyPath;                 ///< 対象プロパティのパス（"position.x" など）
    std::vector<AnimationKeyframe> keyframes; ///< キーフレームリスト
};

/**
 * @class AnimationClip
 * @brief アニメーションデータを格納するアセットクラス
 */
class AnimationClip : public IAsset {
public:
    /**
     * @struct MetaData
     * @brief アニメーションクリップアセット固有のメタデータ構造体
     */
    struct MetaData {
        /**
         * @brief メタデータのシリアライズを行います（現在は空実装）。
         */
        void to_json(nlohmann::json& /*_j*/) const {}
        /**
         * @brief メタデータのデシリアライズを行います（現在は空実装）。
         */
        void from_json(const nlohmann::json& /*_j*/) {}
    };

    /**
     * @brief デフォルトコンストラクタ
     */
    AnimationClip() = default;

    /**
     * @brief デストラクタ
     */
    ~AnimationClip() override = default;

    std::string name;                   ///< アニメーションクリップの名前
    float duration = 0.0f;              ///< 再生時間（秒）※非推奨。endFrameベースに移行予定
    int startFrame = 0;                 ///< 再生開始フレーム数
    int endFrame = 60;                  ///< 再生終了フレーム数
    bool isLooping = false;             ///< ループ再生するかどうかのフラグ
    std::vector<AnimationTrack> tracks; ///< クリップに含まれる全トラックのリスト
};

} /// namespace ONEngine::Asset
