#include "Animator.h"
#include "Engine/Core/Utility/Tools/Log.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Asset/Collection/AssetCollection.h"

/// externals
#include <imgui.h>
#include <string>

namespace ONEngine {

Animator::Animator() {
    Console::LogInfo("Animator: Component created.");
}

void Animator::Play(uint32_t _clipId, uint32_t _layerIndex) {
    if (_layerIndex >= MAX_ANIMATION_LAYERS) return;

    AnimationLayer& layer = layers[_layerIndex];

    // 即時切り替え
    layer.states[0].clipId = _clipId;
    layer.states[0].time = 0.0f;
    layer.states[0].prevTime = 0.0f;
    layer.states[0].weight = 1.0f;
    
    layer.states[1].clipId = 0;
    layer.states[1].weight = 0.0f;

    layer.transitionDuration = 0.0f;
    layer.transitionTimer = 0.0f;
}

void Animator::CrossFade(uint32_t _clipId, float _duration, uint32_t _layerIndex) {
    if (_layerIndex >= MAX_ANIMATION_LAYERS) return;

    Console::LogInfo(std::format("Animator::CrossFade - ClipId: {}, Duration: {}, Layer: {}", _clipId, _duration, _layerIndex));

    AnimationLayer& layer = layers[_layerIndex];

    // すでに同じクリップを再生しようとしている場合は無視（またはリセット）
    if (layer.states[0].clipId == _clipId && layer.transitionDuration <= 0.0f) {
        return;
    }

    // 現在再生中のものを states[1] に移動し、新しいものを states[0] に設定
    // states[1] がフェードアウト、states[0] がフェードイン
    layer.states[1] = layer.states[0];
    
    layer.states[0].clipId = _clipId;
    layer.states[0].time = 0.0f;
    layer.states[0].prevTime = 0.0f;
    layer.states[0].weight = 0.0f;

    layer.transitionDuration = _duration;
    layer.transitionTimer = 0.0f;
}

void Animator::SetPlaybackSpeed(float _speed, uint32_t _layerIndex) {
    if (_layerIndex >= MAX_ANIMATION_LAYERS) return;
    layers[_layerIndex].states[0].playbackSpeed = _speed;
}

void Animator::SetLoop(bool _isLoop, uint32_t _layerIndex) {
    if (_layerIndex >= MAX_ANIMATION_LAYERS) return;
    layers[_layerIndex].states[0].isLoop = _isLoop;
}

float Animator::GetAnimationDuration(uint32_t _clipId) const {
    if (_clipId == 0) return 0.0f;

    auto* owner = GetOwner();
    if (!owner) return 0.0f;

    auto* ecs = owner->GetECSGroup();
    if (!ecs) return 0.0f;

    auto* smr = owner->GetComponent<SkinMeshRenderer>();
    if (!smr) {
        // static int noSmrLog = 0;
        // if (noSmrLog < 1) { Console::LogWarning(std::format("Animator: SkinMeshRenderer not found on owner '{}'", owner->GetName())); noSmrLog++; }
        return 0.0f;
    }

    auto* assetCollection = Asset::AssetCollection::GetInstance();
    if (!assetCollection) return 0.0f;

    auto* model = assetCollection->GetModel(smr->GetMeshPath());
    if (!model) {
        static std::unordered_map<std::string, bool> loggedMissing;
        if (!loggedMissing[smr->GetMeshPath()]) {
            Console::LogError(std::format("Animator::GetAnimationDuration - Model not found at path: {}", smr->GetMeshPath()));
            loggedMissing[smr->GetMeshPath()] = true;
        }
        return 0.0f;
    }

    const auto& clips = model->GetAnimationClips();
    
    // 全クリップのログ出力（デバッグ用）
    static std::unordered_map<std::string, bool> loggedClips;
    if (!loggedClips[smr->GetMeshPath()]) {
        Console::LogInfo(std::format("Animator: Clips for model '{}' (Total: {}):", smr->GetMeshPath(), clips.size()));
        for (const auto& [hash, clip] : clips) {
            Console::LogInfo(std::format("  - '{}' (hash: {})", clip.name, hash));
        }
        loggedClips[smr->GetMeshPath()] = true;
    }

    auto it = clips.find(_clipId);
    if (it != clips.end()) {
        static int foundLogCount = 0;
        if (foundLogCount < 20) {
            Console::LogInfo(std::format("Animator::GetAnimationDuration - SUCCESS: ClipId {} ('{}'), Duration: {}", _clipId, it->second.name, it->second.duration));
            foundLogCount++;
        }
        return it->second.duration;
    }

    static int failLogCount = 0;
    if (failLogCount < 10) {
        Console::LogWarning(std::format("Animator::GetAnimationDuration - FAIL: ClipId {} NOT FOUND in model '{}' ({} clips total)", _clipId, smr->GetMeshPath(), clips.size()));
        failLogCount++;
    }
    return 0.0f;
}

void from_json(const nlohmann::json& _j, Animator& _animator) {
    if (_j.contains("enable")) {
        _animator.enable = _j.at("enable").get<int>();
    }

    if (_j.contains("defaultClipId")) {
        _animator.defaultClipId = _j.at("defaultClipId").get<uint32_t>();
    }

    if (_j.contains("layers")) {
        const auto& layersJson = _j.at("layers");
        for (uint32_t i = 0; i < (uint32_t)layersJson.size() && i < MAX_ANIMATION_LAYERS; ++i) {
            const auto& layerJson = layersJson[i];
            AnimationLayer& layer = _animator.layers[i];

            if (layerJson.contains("weight")) layer.weight = layerJson.at("weight").get<float>();
            if (layerJson.contains("boneMaskHash")) layer.boneMaskHash = layerJson.at("boneMaskHash").get<uint32_t>();

            if (layerJson.contains("states")) {
                const auto& statesJson = layerJson.at("states");
                for (uint32_t j = 0; j < (uint32_t)statesJson.size() && j < MAX_ANIMATION_STATES_PER_LAYER; ++j) {
                    const auto& stateJson = statesJson[j];
                    AnimationState& state = layer.states[j];

                    if (stateJson.contains("clipId")) state.clipId = stateJson.at("clipId").get<uint32_t>();
                    if (stateJson.contains("time")) state.time = stateJson.at("time").get<float>();
                    if (stateJson.contains("weight")) state.weight = stateJson.at("weight").get<float>();
                    if (stateJson.contains("isLoop")) state.isLoop = stateJson.at("isLoop").get<bool>();
                    if (stateJson.contains("playbackSpeed")) state.playbackSpeed = stateJson.at("playbackSpeed").get<float>();
                }
            }
        }
    }
}

void to_json(nlohmann::json& _j, const Animator& _animator) {
    _j = nlohmann::json{
        { "type", "Animator" },
        { "enable", _animator.enable },
        { "defaultClipId", _animator.defaultClipId }
    };

    auto layersJson = nlohmann::json::array();
    for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
        const AnimationLayer& layer = _animator.layers[i];

        auto layerJson = nlohmann::json{
            { "weight", layer.weight },
            { "boneMaskHash", layer.boneMaskHash }
        };

        auto statesJson = nlohmann::json::array();
        for (uint32_t j = 0; j < MAX_ANIMATION_STATES_PER_LAYER; ++j) {
            const AnimationState& state = layer.states[j];
            statesJson.push_back(nlohmann::json{
                { "clipId", state.clipId },
                { "time", state.time },
                { "weight", state.weight },
                { "isLoop", state.isLoop },
                { "playbackSpeed", state.playbackSpeed }
            });
        }
        layerJson["states"] = statesJson;
        layersJson.push_back(layerJson);
    }
    _j["layers"] = layersJson;
}

void Internal_Play(uint64_t _nativeHandle, uint32_t _clipId, uint32_t _layerIndex) {
    Animator* animator = reinterpret_cast<Animator*>(_nativeHandle);
    if (animator) {
        animator->Play(_clipId, _layerIndex);
    }
}

void Internal_CrossFade(uint64_t _nativeHandle, uint32_t _clipId, float _duration, uint32_t _layerIndex) {
    Animator* animator = reinterpret_cast<Animator*>(_nativeHandle);
    if (animator) {
        animator->CrossFade(_clipId, _duration, _layerIndex);
    }
}

void Internal_SetPlaybackSpeed(uint64_t _nativeHandle, float _speed, uint32_t _layerIndex) {
    Animator* animator = reinterpret_cast<Animator*>(_nativeHandle);
    if (animator) {
        animator->SetPlaybackSpeed(_speed, _layerIndex);
    }
}

void Internal_SetLoop(uint64_t _nativeHandle, bool _isLoop, uint32_t _layerIndex) {
    Animator* animator = reinterpret_cast<Animator*>(_nativeHandle);
    if (animator) {
        animator->SetLoop(_isLoop, _layerIndex);
    }
}

float Internal_GetAnimationDuration(uint64_t _nativeHandle, uint32_t _clipId) {
    Animator* animator = reinterpret_cast<Animator*>(_nativeHandle);
    if (animator) {
        return animator->GetAnimationDuration(_clipId);
    }
    return 0.0f;
}

namespace ComponentDebug {
    void AnimatorDebug(Animator* _animator) {
        std::vector<Animator*> animators = { _animator };
        AnimatorDebug(animators);
    }

    void AnimatorDebug(const std::vector<Animator*>& _animators) {
        if (_animators.empty()) return;

        Animator* first = _animators[0];

        // デフォルトクリップの表示
        {
            std::string defaultClipName = "None";
            auto* owner = first->GetOwner();
            auto* smr = owner ? owner->GetComponent<SkinMeshRenderer>() : nullptr;
            if (smr) {
                auto* ac = Asset::AssetCollection::GetInstance();
                if (auto* model = ac->GetModel(smr->GetMeshPath())) {
                    auto it = model->GetAnimationClips().find(first->GetDefaultClip());
                    if (it != model->GetAnimationClips().end()) defaultClipName = it->second.name;
                }
            }
            ImGui::Text("Default Clip: %s (ID: %u)", defaultClipName.c_str(), first->GetDefaultClip());
            if (ImGui::Button("Play Default")) {
                for (auto a : _animators) a->Play(a->GetDefaultClip());
            }
            ImGui::Separator();
        }

        if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
                AnimationLayer& layer = first->layers[i];
                std::string headerName = "Layer " + std::to_string(i);

                if (ImGui::TreeNode(headerName.c_str())) {
                    bool isEdit = false;
                    float weight = layer.weight;

                    if (ImGui::DragFloat("weight", &weight, 0.01f, 0.0f, 1.0f)) {
                        isEdit = true;
                    }

                    if (isEdit) {
                        for (auto a : _animators) {
                            a->layers[i].weight = weight;
                        }
                    }

                    if (ImGui::TreeNode("State 0 (Current)")) {
                        // クリップ名の特定
                        std::string clipName = "Unknown";
                        auto* owner = first->GetOwner();
                        auto* smr = owner ? owner->GetComponent<SkinMeshRenderer>() : nullptr;
                        if (smr) {
                            auto* ac = Asset::AssetCollection::GetInstance();
                            if (auto* model = ac->GetModel(smr->GetMeshPath())) {
                                auto it = model->GetAnimationClips().find(layer.states[0].clipId);
                                if (it != model->GetAnimationClips().end()) clipName = it->second.name;
                            }
                        }

                        ImGui::Text("Active Clip: %s", clipName.c_str());
                        ImGui::Text("Clip ID: %u", layer.states[0].clipId);
                        ImGui::Text("Time: %.2f", layer.states[0].time);
                        ImGui::Text("Weight: %.2f", layer.states[0].weight);
                        
                        bool isLoop = layer.states[0].isLoop;
                        if (ImGui::Checkbox("Loop", &isLoop)) {
                            for (auto a : _animators) a->layers[i].states[0].isLoop = isLoop;
                        }

                        float speed = layer.states[0].playbackSpeed;
                        if (ImGui::DragFloat("Speed", &speed, 0.01f)) {
                            for (auto a : _animators) a->layers[i].states[0].playbackSpeed = speed;
                        }
                        
                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }
            }
        }
    }
}

} // namespace ONEngine
