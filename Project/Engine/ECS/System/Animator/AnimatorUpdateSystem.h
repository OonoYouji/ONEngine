#pragma once

/// std
#include <optional>
#include <cstdint>
#include <unordered_map>

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/Asset/Assets/Mesh/Skinning.h"

namespace ONEngine {

/**
 * @class AnimatorUpdateSystem
 * @brief Animatorコンポーネントによるボーン・関節アニメーションのブレンド、再生状態の進行、およびSkinMeshRendererへの適用を行うシステムクラス
 */
class AnimatorUpdateSystem : public ECSISystem {
public:
    /**
     * @brief コンストラクタ
     */
    AnimatorUpdateSystem() = default;

    /**
     * @brief デストラクタ
     */
    ~AnimatorUpdateSystem() override = default;

    /**
     * @brief 毎フレームのAnimator更新処理およびスキンメッシュボーン階層への適用処理を実行します。
     * @param _ecs 対象となるECSグループへのポインタ
     */
    void RuntimeUpdate(class ECSGroup* _ecs) override;

    /**
     * @brief ボーンマスク（特定の関節にのみアニメーションを適用するフィルタ設定）をシステムに登録します。
     * @param _boneMask 登録するボーンマスク構造体
     */
    static void RegisterBoneMask(const BoneMask& _boneMask);

private:
    /**
     * @brief ボーン階層を再帰的に巡回し、ローカル姿勢からグローバルワールド行列を計算して更新します。
     * @param _smr 対象のスキンメッシュレンダラーへのポインタ
     * @param _jointIndex 処理する関節インデックス
     * @param _parentIndex 親関節のインデックス
     */
    void UpdateSkeletonRecursive(class SkinMeshRenderer* _smr, int32_t _jointIndex, const std::optional<int32_t>& _parentIndex);

    /**
     * @brief 各ジョイント行列をGPUスキンクラスターバッファ（定数/構造化バッファ）へ転送可能な形式に書き込み同期します。
     * @param _smr 対象のスキンメッシュレンダラーへのポインタ
     */
    void UpdateSkinCluster(class SkinMeshRenderer* _smr);

    static std::unordered_map<uint32_t, BoneMask> boneMasks_;
};

} // namespace ONEngine
