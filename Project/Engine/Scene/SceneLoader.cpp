#include "SceneLoader.h"
#include "Externals/nlohmann/json.hpp"
#include "Engine/Common/Console.h"
#include "Schema/Schema.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Script/ScriptHost.h"
#include <fstream>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace Engine::Scene {

namespace {
    // スクリプト追加用のデリゲート（キャッシュ）
    void(*gAddScriptDelegate)(uint32_t, const char*, const char*) = nullptr;

    void EnsureScriptDelegate() {
        if (gAddScriptDelegate) return;
        auto& host = Engine::Script::ScriptHost::GetInstance();
        gAddScriptDelegate = (void(*)(uint32_t, const char*, const char*))host.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"AddScriptByName",
            L"");
    }

    // --- コンポーネントのデシリアライズ関数 ---

    void DeserializeTransform(const json& j, Engine::ECS::Transform& t) {
        try {
            if (j.contains("position") && j["position"].is_object()) {
                t.position.x = j["position"].value("x", 0.0f);
                t.position.y = j["position"].value("y", 0.0f);
                t.position.z = j["position"].value("z", 0.0f);
            }
            if (j.contains("rotation") && j["rotation"].is_object()) {
                t.rotation.x = j["rotation"].value("x", 0.0f);
                t.rotation.y = j["rotation"].value("y", 0.0f);
                t.rotation.z = j["rotation"].value("z", 0.0f);
            }
            if (j.contains("scale") && j["scale"].is_object()) {
                t.scale.x = j["scale"].value("x", 1.0f);
                t.scale.y = j["scale"].value("y", 1.0f);
                t.scale.z = j["scale"].value("z", 1.0f);
            }
        } catch (const std::exception& e) {
            Engine::Console::LogError(std::format("SceneLoader: Error deserializing Transform: {}", e.what()));
        }
    }

    void DeserializeMeshRenderer(const json& j, Engine::ECS::MeshRenderer& mr) {
        try {
            if (j.contains("meshPath") && j["meshPath"].is_string()) {
                mr.modelIndex = Engine::Asset::AssetManager::GetInstance().LoadModel(j["meshPath"]);
            } else if (j.contains("modelIndex") && j["modelIndex"].is_number()) {
                mr.modelIndex = j["modelIndex"];
            }

            if (j.contains("materialPath") && j["materialPath"].is_string()) {
                mr.materialIndex = Engine::Asset::MaterialManager::GetInstance().LoadMaterial(j["materialPath"]);
            } else if (j.contains("materialIndex") && j["materialIndex"].is_number()) {
                mr.materialIndex = j["materialIndex"];
            }
        } catch (const std::exception& e) {
            Engine::Console::LogError(std::format("SceneLoader: Error deserializing MeshRenderer: {}", e.what()));
        }
    }

    void DeserializeCamera(const json& j, Engine::ECS::Camera& c) {
        c.fov = j.value("fov", 45.0f);
        c.nearZ = j.value("nearZ", 0.1f);
        c.farZ = j.value("farZ", 1000.0f);
    }

    void DeserializeDirectionalLight(const json& j, Engine::ECS::DirectionalLight& l) {
        if (j.contains("color")) {
            l.color.x = j["color"].value("x", 1.0f);
            l.color.y = j["color"].value("y", 1.0f);
            l.color.z = j["color"].value("z", 1.0f);
        }
        l.intensity = j.value("intensity", 1.0f);
        if (j.contains("direction")) {
            l.direction.x = j["direction"].value("x", 0.0f);
            l.direction.y = j["direction"].value("y", -1.0f);
            l.direction.z = j["direction"].value("z", 1.0f);
        }
    }

    void DeserializePointLight(const json& j, Engine::ECS::PointLight& l) {
        if (j.contains("color")) {
            l.color.x = j["color"].value("x", 1.0f);
            l.color.y = j["color"].value("y", 1.0f);
            l.color.z = j["color"].value("z", 1.0f);
        }
        l.intensity = j.value("intensity", 1.0f);
        l.radius = j.value("radius", 10.0f);
    }

    void DeserializeEntity(const json& jEntity, Engine::ECS::Registry& registry) {
        auto entity = registry.CreateEntity();
        
        if (jEntity.contains("components") && jEntity["components"].is_array()) {
            for (const auto& jComp : jEntity["components"]) {
                if (!jComp.contains("type") || !jComp["type"].is_string()) continue;
                std::string type = jComp["type"];
                
                if (type == "Transform") {
                    auto& t = registry.AddComponent<Engine::ECS::Transform>(entity);
                    DeserializeTransform(jComp, t);
                } else if (type == "MeshRenderer") {
                    auto& mr = registry.AddComponent<Engine::ECS::MeshRenderer>(entity);
                    DeserializeMeshRenderer(jComp, mr);
                } else if (type == "Camera") {
                    auto& c = registry.AddComponent<Engine::ECS::Camera>(entity);
                    DeserializeCamera(jComp, c);
                } else if (type == "DirectionalLight") {
                    auto& l = registry.AddComponent<Engine::ECS::DirectionalLight>(entity);
                    DeserializeDirectionalLight(jComp, l);
                } else if (type == "PointLight") {
                    auto& l = registry.AddComponent<Engine::ECS::PointLight>(entity);
                    DeserializePointLight(jComp, l);
                } else if (type == "Script") {
                    EnsureScriptDelegate();
                    if (gAddScriptDelegate && jComp.contains("scripts") && jComp["scripts"].is_array()) {
                        for (const auto& jScript : jComp["scripts"]) {
                            if (jScript.contains("name") && jScript["name"].is_string()) {
                                std::string scriptName = jScript["name"];
                                std::string varsJson = "{}";
                                if (jScript.contains("variables") && jScript["variables"].is_object()) {
                                    varsJson = jScript["variables"].dump();
                                }
                                gAddScriptDelegate(entity, scriptName.c_str(), varsJson.c_str());
                            }
                        }
                    }
                }
            }
        }
    }
}

bool SceneLoader::LoadScene(const std::string& path, Engine::ECS::Registry& registry) {
    Engine::Console::Log(std::format("SceneLoader: Loading scene from {}", path));
    
    std::ifstream file(path, std::ios::binary); // バイナリモードで開く
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("Failed to open scene file: {}", path));
        return false;
    }

    // ファイル内容を文字列に読み込む
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // UTF-8 BOM (0xEF, 0xBB, 0xBF) の除去
    if (content.size() >= 3 && 
        (unsigned char)content[0] == 0xEF && 
        (unsigned char)content[1] == 0xBB && 
        (unsigned char)content[2] == 0xBF) {
        content.erase(0, 3);
    }

    try {
        json data = json::parse(content);
        if (data.contains("entities") && data["entities"].is_array()) {
            for (const auto& jEntity : data["entities"]) {
                DeserializeEntity(jEntity, registry);
            }
        }
        Engine::Console::Log(std::format("SceneLoader: Successfully loaded scene {}", path));
        return true;
    } catch (const json::parse_error& e) {
        Engine::Console::LogError(std::format("JSON Parse Error in {}: {} (at byte {})", path, e.what(), e.byte));
        // 問題箇所の周辺を表示するためのデバッグ情報
        size_t start = (e.byte > 20) ? e.byte - 20 : 0;
        std::string context = content.substr(start, 40);
        Engine::Console::LogError(std::format("Context around error: ... {} ...", context));
        return false;
    } catch (const std::exception& e) {
        Engine::Console::LogError(std::format("Error loading scene file {}: {}", path, e.what()));
        return false;
    }
}

Engine::ECS::Entity SceneLoader::InstantiatePrefab(const std::string& path, Engine::ECS::Registry& registry) {
    std::ifstream file(path);
    if (!file.is_open()) return Engine::ECS::kNullEntity;

    try {
        json jEntity = json::parse(file);
        auto entity = registry.CreateEntity();
        
        if (jEntity.contains("components") && jEntity["components"].is_array()) {
            for (const auto& jComp : jEntity["components"]) {
                if (!jComp.contains("type") || !jComp["type"].is_string()) continue;
                std::string type = jComp["type"];
                if (type == "Transform") {
                    auto& t = registry.AddComponent<Engine::ECS::Transform>(entity);
                    DeserializeTransform(jComp, t);
                } else if (type == "MeshRenderer") {
                    auto& mr = registry.AddComponent<Engine::ECS::MeshRenderer>(entity);
                    DeserializeMeshRenderer(jComp, mr);
                } else if (type == "Camera") {
                    auto& c = registry.AddComponent<Engine::ECS::Camera>(entity);
                    DeserializeCamera(jComp, c);
                } else if (type == "DirectionalLight") {
                    auto& l = registry.AddComponent<Engine::ECS::DirectionalLight>(entity);
                    DeserializeDirectionalLight(jComp, l);
                } else if (type == "PointLight") {
                    auto& l = registry.AddComponent<Engine::ECS::PointLight>(entity);
                    DeserializePointLight(jComp, l);
                } else if (type == "Script") {
                    EnsureScriptDelegate();
                    if (gAddScriptDelegate && jComp.contains("scripts") && jComp["scripts"].is_array()) {
                        for (const auto& jScript : jComp["scripts"]) {
                            if (jScript.contains("name") && jScript["name"].is_string()) {
                                std::string scriptName = jScript["name"];
                                std::string varsJson = "{}";
                                if (jScript.contains("variables") && jScript["variables"].is_object()) {
                                    varsJson = jScript["variables"].dump();
                                }
                                gAddScriptDelegate(entity, scriptName.c_str(), varsJson.c_str());
                            }
                        }
                    }
                }
            }
        }
        return entity;
    } catch (...) {
        return Engine::ECS::kNullEntity;
    }
}

} // namespace Engine::Scene
