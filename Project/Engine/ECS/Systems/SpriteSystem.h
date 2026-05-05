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
            
            // 半径（Sizeの半分）をスケールとして使用することで、JSONのsizeが実サイズになるようにする
            Engine::Math::Vector3 scale = {renderer.size.x * 0.5f, renderer.size.y * 0.5f, 1.0f};

            if (renderer.isBillboard) {
                // ビルボードの場合は回転を一旦ゼロにする（シェーダー側でカメラを向ける）
                data.world = Engine::Math::Matrix4x4::MakeAffine(scale, {0, 0, 0}, transform.position);
                data.isBillboard = 1;
            } else {
                data.world = Engine::Math::Matrix4x4::MakeAffine(scale, transform.rotation, transform.position);
                data.isBillboard = 0;
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
