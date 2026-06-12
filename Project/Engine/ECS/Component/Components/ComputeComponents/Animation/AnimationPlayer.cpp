#include "AnimationPlayer.h"

/// external
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/DissolveMeshRenderer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ParticleSystem/ParticleSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/DissolveMeshRenderer.h"
#include <functional>

/// editor
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/AssetDebugger.h"
#include "Engine/Editor/Math/AssetPayload.h"

using namespace ONEngine;

namespace {
    // --- Property Registry for Cleaner Binding ---
    struct PropertyDef {
        AnimationPlayer::PropertyBinding::Type type;
        std::function<void*(IComponent*)> getPtr;
    };

    std::unordered_map<std::string, std::unordered_map<std::string, PropertyDef>> g_PropRegistry;
    bool g_RegistryInitialized = false;

    void InitializeRegistry() {
        if (g_RegistryInitialized) return;
        g_RegistryInitialized = true;

        using Type = AnimationPlayer::PropertyBinding::Type;

        // --- Transform ---
        auto& t = g_PropRegistry["Transform"];
        t["position"] = { Type::Vector3, [](IComponent* c) { return &static_cast<Transform*>(c)->position; } };
        t["position.x"] = { Type::Float, [](IComponent* c) { return &static_cast<Transform*>(c)->position.x; } };
        t["position.y"] = { Type::Float, [](IComponent* c) { return &static_cast<Transform*>(c)->position.y; } };
        t["position.z"] = { Type::Float, [](IComponent* c) { return &static_cast<Transform*>(c)->position.z; } };
        t["rotation"] = { Type::TransformRotationEuler, [](IComponent* c) { return &static_cast<Transform*>(c)->rotate; } };
        t["scale"] = { Type::Vector3, [](IComponent* c) { return &static_cast<Transform*>(c)->scale; } };
        t["scale.x"] = { Type::Float, [](IComponent* c) { return &static_cast<Transform*>(c)->scale.x; } };
        t["scale.y"] = { Type::Float, [](IComponent* c) { return &static_cast<Transform*>(c)->scale.y; } };
        t["scale.z"] = { Type::Float, [](IComponent* c) { return &static_cast<Transform*>(c)->scale.z; } };

        // --- MeshRenderer ---
        auto& mr = g_PropRegistry["MeshRenderer"];
        mr["material.uvTransform.position"] = { Type::Vector2, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.position; } };
        mr["material.uvTransform.position.x"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.position.x; } };
        mr["material.uvTransform.position.y"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.position.y; } };
        mr["material.uvTransform.scale"] = { Type::Vector2, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.scale; } };
        mr["material.uvTransform.rotate"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.rotate; } };
        mr["material.baseColor"] = { Type::Vector4, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().baseColor; } };
        mr["material.baseColor.r"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().baseColor.x; } };
        mr["material.baseColor.g"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().baseColor.y; } };
        mr["material.baseColor.b"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().baseColor.z; } };
        mr["material.baseColor.a"] = { Type::Float, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().baseColor.w; } };
        mr["color"] = mr["material.baseColor"];
        mr["color.r"] = mr["material.baseColor.r"];
        mr["color.g"] = mr["material.baseColor.g"];
        mr["color.b"] = mr["material.baseColor.b"];
        mr["color.a"] = mr["material.baseColor.a"];
        mr["Color"] = mr["color"]; // 大文字対応
        mr["uvOffset"] = mr["material.uvTransform.position"];
        mr["uvScale"] = mr["material.uvTransform.scale"];
        mr["uvRotation"] = mr["material.uvTransform.rotate"];
        mr["postEffectFlags"] = { Type::Int, [](IComponent* c) { return &static_cast<MeshRenderer*>(c)->GetMaterialForAnimation().postEffectFlags; } };

        // --- DissolveMeshRenderer ---
        auto& dmr = g_PropRegistry["DissolveMeshRenderer"];
        dmr["material.uvTransform.position"] = { Type::Vector2, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.position; } };
        dmr["material.uvTransform.scale"] = { Type::Vector2, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.scale; } };
        dmr["material.uvTransform.rotate"] = { Type::Float, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetMaterialForAnimation().uvTransform.rotate; } };
        dmr["material.baseColor"] = { Type::Vector4, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetMaterialForAnimation().baseColor; } };
        dmr["threshold"] = { Type::Float, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetThresholdForAnimation(); } };
        dmr["edgeWidth"] = { Type::Float, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetEdgeWidthForAnimation(); } };
        dmr["edgeColor"] = { Type::Vector4, [](IComponent* c) { return &static_cast<DissolveMeshRenderer*>(c)->GetEdgeColorForAnimation(); } };
        dmr["color"] = dmr["material.baseColor"];
        dmr["uvOffset"] = dmr["material.uvTransform.position"];
        dmr["uvScale"] = dmr["material.uvTransform.scale"];
        dmr["uvRotation"] = dmr["material.uvTransform.rotate"];

        // --- SpriteRenderer ---
        auto& sr = g_PropRegistry["SpriteRenderer"];
        sr["material.uvTransform.position"] = { Type::Vector2, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().uvTransform.position; } };
        sr["material.uvTransform.scale"] = { Type::Vector2, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().uvTransform.scale; } };
        sr["material.uvTransform.rotate"] = { Type::Float, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().uvTransform.rotate; } };
        sr["material.baseColor"] = { Type::Vector4, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().baseColor; } };
        sr["material.baseColor.r"] = { Type::Float, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().baseColor.x; } };
        sr["material.baseColor.g"] = { Type::Float, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().baseColor.y; } };
        sr["material.baseColor.b"] = { Type::Float, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().baseColor.z; } };
        sr["material.baseColor.a"] = { Type::Float, [](IComponent* c) { return &static_cast<SpriteRenderer*>(c)->GetMaterialForAnimation().baseColor.w; } };
        sr["color"] = sr["material.baseColor"];
        sr["color.r"] = sr["material.baseColor.r"];
        sr["color.g"] = sr["material.baseColor.g"];
        sr["color.b"] = sr["material.baseColor.b"];
        sr["color.a"] = sr["material.baseColor.a"];
        sr["Color"] = sr["color"];
        sr["uvOffset"] = sr["material.uvTransform.position"];
        sr["uvScale"] = sr["material.uvTransform.scale"];
        sr["uvRotation"] = sr["material.uvTransform.rotate"];

        // --- Light ---
        auto& l = g_PropRegistry["DirectionalLight"];
        l["color"] = { Type::Vector4, [](IComponent* c) { return &static_cast<DirectionalLight*>(c)->GetColorForAnimation(); } };
        l["intensity"] = { Type::Float, [](IComponent* c) { return &static_cast<DirectionalLight*>(c)->GetIntensityForAnimation(); } };
        g_PropRegistry["Light"] = l;

        // --- ParticleSystem ---
        auto& ps = g_PropRegistry["ParticleSystem"];
        ps["main.startLifetime"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->main.startLifetime.constant; } };
        ps["main.startSpeed"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->main.startSpeed.constant; } };
        ps["main.startSize"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->main.startSize.constant; } };
        ps["main.startRotation"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->main.startRotation.constant; } };
        ps["main.startColor"] = { Type::Vector4, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->main.startColor.constant; } };
        ps["main.gravityModifier"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->main.gravityModifier; } };
        ps["emission.enabled"] = { Type::Bool, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->emission.enabled; } };
        ps["emission.rateOverTime"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->emission.rateOverTime; } };
        ps["shape.radius"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->shape.radius; } };
        ps["shape.angle"] = { Type::Float, [](IComponent* c) { return &static_cast<ParticleSystem*>(c)->shape.angle; } };
    }
}

AnimationPlayer::AnimationPlayer() {
    Reset();
    InitializeRegistry();
}

AnimationPlayer::~AnimationPlayer() {
    ClearBindings();
}

void AnimationPlayer::Reset() {
    ClearBindings();
    clipPath = "";
    currentTime = 0.0f;
    speed = 1.0f;
    isPlaying = false;
    isLooping = true;
    autoPlay = true;
    isBound = false;
}

void AnimationPlayer::ClearBindings() {
    for (auto& b : bindings) {
        if (b.monoGcHandle != 0) {
            mono_gchandle_free(b.monoGcHandle);
        }
    }
    bindings.clear();
    isBound = false;
}

void AnimationPlayer::Play() {
    isPlaying = true;
    Bind();
}

void AnimationPlayer::Pause() {
    isPlaying = false;
}

void AnimationPlayer::Stop() {
    isPlaying = false;
    
    // クリップ情報を取得して開始時間へ戻す
    auto* ac = Asset::AssetCollection::GetInstance();
    if (auto* clip = ac->GetAsset<Asset::AnimationClip>(ac->GetAssetGuidFromPath(clipPath))) {
        currentTime = clip->startFrame / 60.0f;
    } else {
        currentTime = 0.0f;
    }
    
    shouldApplyOnce = true;
}

void AnimationPlayer::SetClip(const std::string& _path) {
    std::string path = _path;
    std::replace(path.begin(), path.end(), '\\', '/');
    if (!path.starts_with("./") && !path.starts_with("/") && (path.starts_with("Assets") || path.starts_with("Packages"))) {
        path = "./" + path;
    }
    clipPath = path;
    ClearBindings(); // クリップが変わったらバインドをやり直す
}

void AnimationPlayer::Bind() {
    ClearBindings();

    auto* ac = Asset::AssetCollection::GetInstance();
    auto guid = ac->GetAssetGuidFromPath(clipPath);
    auto* clip = ac->GetAsset<Asset::AnimationClip>(guid);

    if (!clip) {
        isBound = false;
        return;
    }

    isBound = true;
    GameEntity* entity = GetOwner();
    if (!entity) return;

    for (const auto& track : clip->tracks) {
        PropertyBinding binding;
        binding.propertyPath = track.propertyPath;
        binding.type = PropertyBinding::Type::None;

        if (track.propertyPath.empty()) {
            bindings.push_back(binding);
            continue;
        }
        
        // --- C# Script direct access ---
        if (track.componentName.find("Script:") == 0 && track.componentName.length() > 7) {
            std::string className = track.componentName.substr(7);
            std::string ecsGroupName = entity->GetECSGroup()->GetGroupName();
            
            MonoObject* scriptInstance = MonoScriptEngine::GetInstance().GetMonoBehaviorFromCS(ecsGroupName, entity->GetId(), className);
            if (scriptInstance) {
                MonoClass* monoClass = mono_object_get_class(scriptInstance);
                MonoClassField* field = MonoScriptEngineUtils::FindFieldRecursive(monoClass, track.propertyPath.c_str());
                
                if (field) {
                    binding.type = PropertyBinding::Type::CSField;
                    binding.monoField = field;
                    binding.monoGcHandle = mono_gchandle_new(scriptInstance, false);
                    bindings.push_back(binding);
                    continue;
                }
            }

            binding.targetComponent = entity->GetComponent<Variables>();
            if (binding.targetComponent) {
                binding.type = PropertyBinding::Type::ScriptVar;
                binding.scriptGroupName = className;
                binding.scriptVarName = track.propertyPath;
            }
            bindings.push_back(binding);
            continue;
        }

        // --- C++ Component access ---
        std::string compName = track.componentName;
        binding.targetComponent = entity->GetComponent(compName);

        // Fallback: If component is "Material" or empty, try to find any renderer
        if (!binding.targetComponent && (compName == "Material" || compName == "Renderer" || compName == "")) {
            binding.targetComponent = entity->GetComponent<MeshRenderer>();
            if (binding.targetComponent) compName = "MeshRenderer";
            else {
                binding.targetComponent = entity->GetComponent<SpriteRenderer>();
                if (binding.targetComponent) compName = "SpriteRenderer";
            }
        }

        if (binding.targetComponent) {
            auto compIt = g_PropRegistry.find(compName);
            if (compIt != g_PropRegistry.end()) {
                auto propIt = compIt->second.find(track.propertyPath);
                if (propIt != compIt->second.end()) {
                    binding.dataPtr = propIt->second.getPtr(binding.targetComponent);
                    binding.type = propIt->second.type;
                }
            }
        }

        bindings.push_back(binding);
    }
}

void ONEngine::from_json(const nlohmann::json& _j, AnimationPlayer& _a) {
    _a.clipPath = _j.value("clipPath", "");
    _a.currentTime = _j.value("currentTime", 0.0f);
    _a.speed = _j.value("speed", 1.0f);
    _a.isPlaying = _j.value("isPlaying", false);
    _a.isLooping = _j.value("isLooping", true);
    _a.autoPlay = _j.value("autoPlay", true);
}

void ONEngine::to_json(nlohmann::json& _j, const AnimationPlayer& _a) {
    _j = nlohmann::json{
        { "type", "AnimationPlayer" },
        { "clipPath", _a.clipPath },
        { "currentTime", _a.currentTime },
        { "speed", _a.speed },
        { "isPlaying", _a.isPlaying },
        { "isLooping", _a.isLooping },
        { "autoPlay", _a.autoPlay }
    };
}

void ComponentDebug::AnimationPlayerDebug(AnimationPlayer* _player) {
    if (!_player) return;

    ImGui::Text("Animation Player");
    
    char pathBuf[256];
    strncpy_s(pathBuf, _player->clipPath.c_str(), sizeof(pathBuf));
    if (ImGui::InputText("Clip Path", pathBuf, sizeof(pathBuf))) {
        _player->SetClip(pathBuf);
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
            if (payload->Data) {
                Editor::AssetPayload* assetPayload = *static_cast<Editor::AssetPayload**>(payload->Data);
                std::string path = assetPayload->filePath;
                _player->SetClip(path);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::DragFloat("Current Time", &_player->currentTime, 0.01f);
    ImGui::DragFloat("Speed", &_player->speed, 0.1f);
    ImGui::Checkbox("Is Playing", &_player->isPlaying);
    ImGui::Checkbox("Is Looping", &_player->isLooping);
    ImGui::Checkbox("Auto Play", &_player->autoPlay);

    if (ImGui::Button("Play")) _player->Play();
    ImGui::SameLine();
    if (ImGui::Button("Pause")) _player->Pause();
    ImGui::SameLine();
    if (ImGui::Button("Stop")) _player->Stop();

    if (ImGui::Button("Force Bind")) _player->Bind();
}
