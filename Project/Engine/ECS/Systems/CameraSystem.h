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
        Engine::Math::Matrix4x4 viewProj;
        Engine::Math::Vector3 position;
    };

    void Update(Registry& registry) override {
        // 最初に見つかった有効なカメラを使用する
        registry.GetView<Transform, Camera>().Each([&](Entity entity, Transform& transform, Camera& camera) {
            if (found_) return;

            // ビュー行列の計算 (Transformの情報を元に)
            // 注: 本来は Quaternion から Forward ベクトルを出すのが望ましいが、現状は Euler (rotation)
            // 簡単のため LookAtLH を使用するか、あるいは Transform の逆行列を使用する
            
            // 1. Transform からワールド行列を作成
            auto world = Engine::Math::Matrix4x4::MakeAffine(transform.scale, transform.rotation, transform.position);
            
            // 2. ビュー行列はワールド行列の逆行列 (Scaleは1と仮定)
            // もしくは回転と平行移動から直接作成
            auto view = Engine::Math::Matrix4x4::MakeLookAtLH(
                transform.position, 
                transform.position + Engine::Math::Vector3{ 
                    sinf(transform.rotation.y) * cosf(transform.rotation.x),
                    -sinf(transform.rotation.x),
                    cosf(transform.rotation.y) * cosf(transform.rotation.x)
                }, 
                {0, 1, 0}
            );
            
            // 3. プロジェクション行列の計算
            auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(
                camera.fov * (3.141592f / 180.0f), 16.0f / 9.0f, camera.nearZ, camera.farZ
            );

            result_.viewProj = view * proj;
            result_.position = transform.position;
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
