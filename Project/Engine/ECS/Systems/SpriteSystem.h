#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Schema.h"
#include "Engine/Core/Math/Math.h"
#include <vector>

namespace Engine::ECS {

///
/// スプライト描画情報を集計するシステム
///
class SpriteSystem final : public System {
public:
    struct SpriteResult {
        std::vector<GeneratedSchema::SpriteData> sprites;
    };

    void Update(Registry& registry) override {
        // カメラの行列を取得（ビルボード計算用）
        // ※ 本来は CameraSystem から取得すべきだが、ここでは簡易的にワールド行列から逆算するか、
        // あるいは SceneData の cameraPos 等を利用する。
        
        registry.GetView<Transform, SpriteRenderer>().Each([&](Entity entity, Transform& transform, SpriteRenderer& renderer) {
            GeneratedSchema::SpriteData data;
            
            if (renderer.isBillboard) {
                // ビルボード: 回転を無視し、カメラの方向を向く行列を作成
                // TODO: 厳密なビルボード計算
                data.world = Engine::Math::Matrix4x4::MakeAffine(
                    {renderer.size.x, renderer.size.y, 1.0f}, 
                    {0, 0, 0}, 
                    transform.position
                );
            } else {
                data.world = Engine::Math::Matrix4x4::MakeAffine(
                    {renderer.size.x, renderer.size.y, 1.0f}, 
                    transform.rotation, 
                    transform.position
                );
            }

            data.color = renderer.color;
            data.textureIndex = renderer.textureIndex;
            result_.sprites.push_back(data);
        });
    }

    const SpriteResult& GetResult() const { return result_; }
    void Reset() { result_.sprites.clear(); }

private:
    SpriteResult result_;
};

} // namespace Engine::ECS
