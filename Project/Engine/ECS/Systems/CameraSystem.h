#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Schema.h"
#include "Engine/Core/Math/Math.h"

namespace Engine::ECS {

///
/// カメラコンポーネントを監視してビュー・プロジェクション行列を計算するシステム
///
class CameraSystem final : public System {
public:
    struct CameraResult {
        Engine::Math::Matrix4x4 view;
        Engine::Math::Matrix4x4 proj;
        Engine::Math::Matrix4x4 viewProj;
        Engine::Math::Vector3 position;
        float nearZ;
        float farZ;
    };

    void Update(Registry& registry) override {
        // 最初に見つかった有効なカメラを使用する
        registry.GetView<Transform, Camera>().Each([&](Entity entity, Transform& transform, Camera& camera) {
            if (found_) return;

            // 1. ビュー行列の計算
            auto view = Engine::Math::Matrix4x4::MakeLookAtLH(
                transform.position, 
                transform.position + Engine::Math::Vector3{ 
                    sinf(transform.rotation.y) * cosf(transform.rotation.x),
                    -sinf(transform.rotation.x),
                    cosf(transform.rotation.y) * cosf(transform.rotation.x)
                }, 
                {0, 1, 0}
            );
            
            // 2. プロジェクション行列の計算
            auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(
                camera.fov * (3.141592f / 180.0f), 16.0f / 9.0f, camera.nearZ, camera.farZ
            );

            result_.view = view;
            result_.proj = proj;
            result_.viewProj = view * proj;
            result_.position = transform.position;
            result_.nearZ = camera.nearZ;
            result_.farZ = camera.farZ;
            found_ = true;
        });
    }

    bool HasCamera() const { return found_; }
    const CameraResult& GetResult() const { return result_; }
    
    void Reset() { found_ = false; }

private:
    bool found_ = false;
    CameraResult result_;
};

} // namespace Engine::ECS
