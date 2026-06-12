#include "AnimationSystem.h"

/// external
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animation/AnimationPlayer.h"
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Math/Interpolation.h"
#include "Engine/Script/MonoScriptEngine.h"

using namespace ONEngine;

void AnimationSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
    Update(_ecs, Time::UnscaledDeltaTime());
}

void AnimationSystem::RuntimeUpdate(ECSGroup* _ecs) {
    Update(_ecs, Time::DeltaTime());
}

namespace {
    // 補間用ヘルパー
    template<typename T>
    T EvaluateTrack(const std::vector<Asset::AnimationKeyframe>& _keyframes, float _time) {
        if (_keyframes.empty()) return T{};
        if (_time <= _keyframes.front().time) return std::get<T>(_keyframes.front().value);
        if (_time >= _keyframes.back().time) return std::get<T>(_keyframes.back().value);

        for (size_t i = 0; i < (int)_keyframes.size() - 1; ++i) {
            if (_time >= _keyframes[i].time && _time < _keyframes[i + 1].time) {
                float t = (_time - _keyframes[i].time) / (_keyframes[i + 1].time - _keyframes[i].time);
                const T& v0 = std::get<T>(_keyframes[i].value);
                const T& v1 = std::get<T>(_keyframes[i + 1].value);
                
                if (_keyframes[i].interpolation == "Step") return Math::Step(v0, v1, t);
                return Math::Lerp(v0, v1, t);
            }
        }
        return std::get<T>(_keyframes.back().value);
    }
}

void AnimationSystem::Update(ECSGroup* _ecs, float _deltaTime) {
    ComponentArray<AnimationPlayer>* playerArray = _ecs->GetComponentArray<AnimationPlayer>();
    if (!playerArray) return;

    auto* ac = Asset::AssetCollection::GetInstance();

    for (auto& player : playerArray->GetUsedComponents()) {
        if (!player || !player->enable) continue;
        
        // 再生中ではない、かつ強制適用フラグも立っていないならスキップ
        if (!player->isPlaying && !player->shouldApplyOnce) continue;

        // 時間の進行（再生中のみ）
        if (player->isPlaying) {
            player->currentTime += _deltaTime * player->speed;
        }

        // クリップの取得
        std::string path = player->clipPath;
        if (!path.empty() && !path.starts_with("./") && !path.starts_with("/") && (path.starts_with("Assets") || path.starts_with("Packages"))) {
            path = "./" + path;
        }

        auto guid = ac->GetAssetGuidFromPath(path);
        auto* clip = ac->GetAsset<Asset::AnimationClip>(guid);
        
        if (!clip) {
            player->shouldApplyOnce = false; // クリップがないならフラグを下ろす
            continue;
        }

        // バインドの確認
        if (!player->isBound || player->bindings.size() != clip->tracks.size()) {
            player->Bind();
        }

        // ループ/終了処理
        float startTime = clip->startFrame / 60.0f;
        float endTime = clip->endFrame / 60.0f;
        float loopDuration = endTime - startTime;

        if (player->isPlaying && player->currentTime > endTime) {
            if (player->isLooping && loopDuration > 0.0f) {
                player->currentTime = startTime + std::fmod(player->currentTime - startTime, loopDuration);
            } else {
                player->currentTime = endTime;
                player->isPlaying = false;
            }
        }
        
        if (player->currentTime < startTime) {
            player->currentTime = startTime;
        }

        // 各トラックの値を適用
        for (size_t i = 0; i < (int)clip->tracks.size() && i < (int)player->bindings.size(); ++i) {
            const auto& track = clip->tracks[i];
            auto& binding = player->bindings[i];

            if (binding.type == AnimationPlayer::PropertyBinding::Type::None) continue;

            if (binding.type == AnimationPlayer::PropertyBinding::Type::CSField) {
                if (!binding.monoField || binding.monoGcHandle == 0) continue;

                MonoObject* obj = mono_gchandle_get_target(binding.monoGcHandle);
                if (!obj) continue;

                if (std::holds_alternative<float>(track.keyframes[0].value)) {
                    float val = EvaluateTrack<float>(track.keyframes, player->currentTime);
                    mono_field_set_value(obj, binding.monoField, &val);
                } else if (std::holds_alternative<Vector2>(track.keyframes[0].value)) {
                    Vector2 val = EvaluateTrack<Vector2>(track.keyframes, player->currentTime);
                    mono_field_set_value(obj, binding.monoField, &val);
                } else if (std::holds_alternative<Vector3>(track.keyframes[0].value)) {
                    Vector3 val = EvaluateTrack<Vector3>(track.keyframes, player->currentTime);
                    mono_field_set_value(obj, binding.monoField, &val);
                } else if (std::holds_alternative<Vector4>(track.keyframes[0].value)) {
                    Vector4 val = EvaluateTrack<Vector4>(track.keyframes, player->currentTime);
                    mono_field_set_value(obj, binding.monoField, &val);
                }
            } else if (binding.type == AnimationPlayer::PropertyBinding::Type::ScriptVar) {
                auto* vars = static_cast<Variables*>(binding.targetComponent);
                if (track.keyframes.empty()) continue;
                
                if (std::holds_alternative<float>(track.keyframes[0].value)) {
                    vars->SetVariable(binding.scriptGroupName, binding.scriptVarName, EvaluateTrack<float>(track.keyframes, player->currentTime));
                } else if (std::holds_alternative<Vector2>(track.keyframes[0].value)) {
                    vars->SetVariable(binding.scriptGroupName, binding.scriptVarName, EvaluateTrack<Vector2>(track.keyframes, player->currentTime));
                } else if (std::holds_alternative<Vector3>(track.keyframes[0].value)) {
                    vars->SetVariable(binding.scriptGroupName, binding.scriptVarName, EvaluateTrack<Vector3>(track.keyframes, player->currentTime));
                } else if (std::holds_alternative<Vector4>(track.keyframes[0].value)) {
                    vars->SetVariable(binding.scriptGroupName, binding.scriptVarName, EvaluateTrack<Vector4>(track.keyframes, player->currentTime));
                }
            } else if (binding.dataPtr) {
                switch (binding.type) {
                case AnimationPlayer::PropertyBinding::Type::Bool:
                {
                    // Evaluate as float and convert to bool (e.g. > 0.5 is true)
                    float floatVal = EvaluateTrack<float>(track.keyframes, player->currentTime);
                    *static_cast<bool*>(binding.dataPtr) = (floatVal >= 0.5f);
                    break;
                }
                case AnimationPlayer::PropertyBinding::Type::Int:
                {
                    // Evaluate as float and cast to int
                    float floatVal = EvaluateTrack<float>(track.keyframes, player->currentTime);
                    *static_cast<int*>(binding.dataPtr) = static_cast<int>(floatVal);
                    break;
                }
                case AnimationPlayer::PropertyBinding::Type::Float:
                    *static_cast<float*>(binding.dataPtr) = EvaluateTrack<float>(track.keyframes, player->currentTime);
                    break;
                case AnimationPlayer::PropertyBinding::Type::Vector2:
                    *static_cast<Vector2*>(binding.dataPtr) = EvaluateTrack<Vector2>(track.keyframes, player->currentTime);
                    break;
                case AnimationPlayer::PropertyBinding::Type::Vector3:
                    *static_cast<Vector3*>(binding.dataPtr) = EvaluateTrack<Vector3>(track.keyframes, player->currentTime);
                    break;
                case AnimationPlayer::PropertyBinding::Type::Vector4:
                    *static_cast<Vector4*>(binding.dataPtr) = EvaluateTrack<Vector4>(track.keyframes, player->currentTime);
                    break;
                case AnimationPlayer::PropertyBinding::Type::TransformRotationEuler:
                {
                    Vector3 euler = EvaluateTrack<Vector3>(track.keyframes, player->currentTime);
                    Vector3 radians = {
                        euler.x * (std::numbers::pi_v<float> / 180.0f),
                        euler.y * (std::numbers::pi_v<float> / 180.0f),
                        euler.z * (std::numbers::pi_v<float> / 180.0f)
                    };
                    *static_cast<Quaternion*>(binding.dataPtr) = Quaternion::FromEuler(radians);
                    break;
                }
                }
            }
        }

        // 強制適用フラグをクリア
        player->shouldApplyOnce = false;
    }
}
