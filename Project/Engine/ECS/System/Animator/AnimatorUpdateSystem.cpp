#include "AnimatorUpdateSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animator/Animator.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Utility/Tools/Log.h"

#include <algorithm>
#include <cmath>

namespace ONEngine {

std::unordered_map<uint32_t, BoneMask> AnimatorUpdateSystem::boneMasks_;

/**
 * @brief ボーンマスク（特定の関節にのみアニメーションを適用するフィルタ設定）をシステムに登録します。
 */
void AnimatorUpdateSystem::RegisterBoneMask(const BoneMask& _boneMask) {
    boneMasks_[_boneMask.nameHash] = _boneMask;
}

/**
 * @brief ボーン階層を再帰的に巡回し、ローカル姿勢からグローバルワールド行列を計算して更新します。
 */
void AnimatorUpdateSystem::UpdateSkeletonRecursive(SkinMeshRenderer* _smr, int32_t _jointIndex, const std::optional<int32_t>& _parentIndex) {
    Skeleton& skeleton = _smr->skeleton_;
    Joint& joint = skeleton.joints[_jointIndex];

    joint.transform.Update();

    // スケルトン空間行列の計算 (Local * Parent)
    if (_parentIndex) {
        joint.matSkeletonSpace = joint.transform.matWorld * skeleton.joints[*_parentIndex].matSkeletonSpace;
    } else {
        joint.matSkeletonSpace = joint.transform.matWorld;
    }

    // ワールド行列の計算
    joint.matWorld = joint.matSkeletonSpace * _smr->GetOwner()->GetTransform()->matWorld;

    // 子の更新
    for (int32_t childIndex : joint.children) {
        UpdateSkeletonRecursive(_smr, childIndex, _jointIndex);
    }
}

/**
 * @brief 各ジョイント行列をGPUスキンクラスターバッファ（定数/構造化バッファ）へ転送可能な形式に書き込み同期します。
 */
void AnimatorUpdateSystem::UpdateSkinCluster(SkinMeshRenderer* _smr) {
    if (!_smr->skinCluster_) return;
    
    Skeleton& skeleton = _smr->skeleton_;
    SkinCluster& skinCluster = _smr->skinCluster_.value();

    for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
        if (jointIndex >= skinCluster.matBindPoseInverseArray.size()) continue;

        skinCluster.mappedPalette[jointIndex].matSkeletonSpace =
            skinCluster.matBindPoseInverseArray[jointIndex] * skeleton.joints[jointIndex].matSkeletonSpace;

        skinCluster.mappedPalette[jointIndex].matSkeletonSpaceInverseTranspose =
            Matrix4x4::MakeTranspose(Matrix4x4::MakeInverse(skinCluster.mappedPalette[jointIndex].matSkeletonSpace));
    }
}

/**
 * @brief 毎フレームのAnimator更新処理およびスキンメッシュボーン階層への適用処理を実行します。
 */
void AnimatorUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
    if (!_ecs) return;

    ComponentArray<Animator>* animatorArray = _ecs->GetComponentArray<Animator>();
    if (!animatorArray || animatorArray->GetUsedComponents().empty()) return;

    Asset::AssetCollection* assetCollection = Asset::AssetCollection::GetInstance();

    for (auto& animator : animatorArray->GetUsedComponents()) {
        if (!animator || !animator->enable || !animator->GetOwner()->active) continue;

        SkinMeshRenderer* skinMesh = animator->GetOwner()->GetComponent<SkinMeshRenderer>();
        
        if (!skinMesh) {
            static std::unordered_map<uint32_t, bool> loggedMissing;
            if (!loggedMissing[animator->GetOwner()->GetId()]) {
                Console::LogError(std::format("AnimatorUpdateSystem: SkinMeshRenderer not found on entity '{}'", animator->GetOwner()->GetName()));
                loggedMissing[animator->GetOwner()->GetId()] = true;
            }
            continue;
        }

        if (skinMesh->skeleton_.joints.empty()) {
            static std::unordered_map<uint32_t, bool> loggedEmpty;
            if (!loggedEmpty[animator->GetOwner()->GetId()]) {
                Console::LogWarning(std::format("AnimatorUpdateSystem: Skeleton is empty on entity '{}'. Waiting for initialization...", animator->GetOwner()->GetName()));
                loggedEmpty[animator->GetOwner()->GetId()] = true;
            }
            continue;
        }

        Asset::Model* model = assetCollection->GetModel(skinMesh->GetMeshPath());
        if (!model) {
            static std::unordered_map<uint32_t, bool> loggedModelMissing;
            if (!loggedModelMissing[animator->GetOwner()->GetId()]) {
                Console::LogError(std::format("AnimatorUpdateSystem: Model not found at path '{}' for entity '{}'", skinMesh->GetMeshPath(), animator->GetOwner()->GetName()));
                loggedModelMissing[animator->GetOwner()->GetId()] = true;
            }
            continue;
        }

        const auto& clips = model->GetAnimationClips();

        // 0. 何も再生されていない場合、デフォルトクリップを適用
        if (animator->layers[0].states[0].clipId == 0 && animator->defaultClipId != 0) {
            animator->Play(animator->defaultClipId);
        }

        bool isAnyStateActive = false;

        // 1. タイムラインの進行とウェイトの更新 (クロスフェード)
        for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
            AnimationLayer& layer = animator->layers[i];
            
            // トランジション（クロスフェード）の更新
            if (layer.transitionDuration > 0.0f) {
                layer.transitionTimer += Time::DeltaTime();
                float t = (std::min)(1.0f, layer.transitionTimer / layer.transitionDuration);
                
                layer.states[0].weight = t;       // フェードイン
                layer.states[1].weight = 1.0f - t; // フェードアウト

                if (t >= 1.0f) {
                    layer.transitionDuration = 0.0f;
                    layer.states[1].clipId = 0;
                    layer.states[1].weight = 0.0f;
                }
            }

            if (layer.weight <= 0.0f) continue;

            for (uint32_t j = 0; j < MAX_ANIMATION_STATES_PER_LAYER; ++j) {
                AnimationState& state = layer.states[j];
                if (state.weight <= 0.0f || state.clipId == 0) continue;

                // durationの取得
                float duration = 0.0f;
                auto itClip = clips.find(state.clipId);
                if (itClip != clips.end()) {
                    duration = itClip->second.duration;
                }

                state.prevTime = state.time;
                state.time += Time::DeltaTime() * state.playbackSpeed;
                
                if (state.isLoop && duration > 0.0f) {
                    state.time = (std::fmod)(state.time, duration);
                } else if (state.time > duration) {
                    state.time = duration;
                }
                isAnyStateActive = true;
            }
        }

        if (isAnyStateActive) {
            Console::LogInfo(std::format("Animator updating entity: {} (ID: {})", 
                animator->GetOwner()->GetName(), animator->GetOwner()->GetId()));
        }

        // 2. ブレンディング
        for (size_t jointIndex = 0; jointIndex < skinMesh->skeleton_.joints.size(); ++jointIndex) {
            Joint& joint = skinMesh->skeleton_.joints[jointIndex];
            
            Vector3 blendedTranslate = { 0,0,0 };
            Quaternion blendedRotate = { 0,0,0,0 }; 
            Vector3 blendedScale = { 0,0,0 };
            float totalWeight = 0.0f;

            for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
                AnimationLayer& layer = animator->layers[i];
                if (layer.weight <= 0.0f) continue;

                // ボーンマスクの重み取得
                float boneWeight = 1.0f;
                if (layer.boneMaskHash != 0) {
                    auto it = boneMasks_.find(layer.boneMaskHash);
                    if (it != boneMasks_.end()) {
                        auto weightIt = it->second.jointWeights.find(joint.nameHash);
                        if (weightIt != it->second.jointWeights.end()) {
                            boneWeight = weightIt->second;
                        } else {
                            // マスクに含まれていないボーンは重み 0
                            boneWeight = 0.0f;
                        }
                    }
                }
                if (boneWeight <= 0.0f) continue;

                for (uint32_t j = 0; j < MAX_ANIMATION_STATES_PER_LAYER; ++j) {
                    AnimationState& state = layer.states[j];
                    if (state.weight <= 0.0f || state.clipId == 0) continue;

                    ANIME_MATH::SampledTransform sampled;
                    sampled.translate = joint.transform.position;
                    sampled.rotate = joint.transform.rotate;
                    sampled.scale = joint.transform.scale;

                    ANIME_MATH::SampleAnimation(clips, state, joint.nameHash, sampled);

                    float finalWeight = layer.weight * state.weight * boneWeight;
                    blendedTranslate += sampled.translate * finalWeight;
                    blendedScale += sampled.scale * finalWeight;
                    
                    if (blendedRotate.Dot(sampled.rotate) < 0.0f) {
                        blendedRotate += (sampled.rotate * -1.0f) * finalWeight;
                    } else {
                        blendedRotate += sampled.rotate * finalWeight;
                    }

                    totalWeight += finalWeight;
                }
            }

            if (totalWeight > 0.1f) {
                joint.transform.position = blendedTranslate / totalWeight;
                joint.transform.scale = blendedScale / totalWeight;
                joint.transform.rotate = Quaternion::Normalize(blendedRotate);

                if (jointIndex == 0 && isAnyStateActive) {
                    Console::LogInfo(std::format("Joint 0 (Root) Update: weight={:.2f}, pos=({:.2f},{:.2f},{:.2f}), rot=({:.2f},{:.2f},{:.2f},{:.2f})", 
                        totalWeight, joint.transform.position.x, joint.transform.position.y, joint.transform.position.z,
                        joint.transform.rotate.x, joint.transform.rotate.y, joint.transform.rotate.z, joint.transform.rotate.w));
                }
            }
        }

        // 階層構造の更新
        UpdateSkeletonRecursive(skinMesh, skinMesh->skeleton_.root, std::nullopt);
        UpdateSkinCluster(skinMesh);

        // 従来のシステムの更新を抑制
        skinMesh->isPlaying_ = false;
    }
}

} // namespace ONEngine
