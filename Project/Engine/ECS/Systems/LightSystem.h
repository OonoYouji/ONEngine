#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Schema.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::ECS {

///
/// シーン内のライトを集計するシステム
///
class LightSystem final : public System {
public:
    struct LightResult {
        Engine::Math::Vector3 dirLightColor = { 1, 1, 1 };
        float dirLightIntensity = 0.0f;
        Engine::Math::Vector3 dirLightDirection = { 0, -1, 1 };
        
        std::vector<GeneratedSchema::PointLightData> pointLights;
    };

    void Update(Registry& registry) override {
        // 最初の方向性ライトのみを取得（簡易版）
        registry.GetView<DirectionalLight>().Each([&](Entity entity, DirectionalLight& light) {
            if (dirLightFound_) return;
            
            result_.dirLightColor = light.color;
            result_.dirLightIntensity = light.intensity;
            result_.dirLightDirection = light.direction;
            dirLightFound_ = true;
        });

        // 点光源を全て収集
        registry.GetView<Transform, PointLight>().Each([&](Entity entity, Transform& transform, PointLight& light) {
            GeneratedSchema::PointLightData data;
            data.position = transform.position;
            data.intensity = light.intensity;
            data.color = light.color;
            data.radius = light.radius;
            result_.pointLights.push_back(data);
        });
    }

    const LightResult& GetResult() const { return result_; }
    void Reset() { 
        dirLightFound_ = false; 
        result_.pointLights.clear();
    }

private:
    bool dirLightFound_ = false;
    LightResult result_;
};

} // namespace Engine::ECS
