#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include "Engine/Core/Math/Math.h"
#include "Engine/Asset/FontManager.h"
#include <vector>
#include <string>

namespace Engine::ECS {

///
/// テキスト描画情報を集計するシステム
///
class TextSystem final : public System {
public:
    struct TextResult {
        std::vector<GeneratedSchema::TextData> charInstances;
    };

    void Update(Registry& registry) override {
        auto& fontManager = Engine::Asset::FontManager::GetInstance();
        uint32_t entityCount = 0;

        registry.GetView<Transform, TextRenderer>().Each([&](Entity entity, Transform& transform, TextRenderer& renderer) {
            entityCount++;
            std::string text(renderer.text);
            if (text.empty()) return;

            auto* font = fontManager.GetFontByIndex(renderer.fontIndex);
            if (!font) return;

            float xOffset = 0.0f;
            float yOffset = 0.0f;

            for (char c : text) {
                if (c == '\0') break;
                if (c == '\n') {
                    xOffset = 0;
                    yOffset -= renderer.size * 1.2f;
                    continue;
                }

                auto* glyph = font->GetGlyph(c);
                if (!glyph) continue;

                GeneratedSchema::TextData data;
                
                // 1. 文字のサイズとオフセットを考慮した配置
                float w = glyph->width * (renderer.size / 32.0f); // 標準32px基準
                float h = glyph->height * (renderer.size / 32.0f);
                float ox = glyph->xOffset * (renderer.size / 32.0f);
                float oy = glyph->yOffset * (renderer.size / 32.0f);

                // 2. ワールド行列の計算
                // 文字の左上原点を考慮
                Engine::Math::Vector3 charPos = transform.position;
                charPos.x += xOffset + ox + w * 0.5f;
                charPos.y += yOffset - oy - h * 0.5f;

                data.world = Engine::Math::Matrix4x4::MakeAffine(
                    {w * 0.5f, h * 0.5f, 1.0f}, 
                    transform.rotation, 
                    charPos
                );

                data.color = renderer.color;
                data.textureIndex = font->GetAtlasTexture()->GetIndex();
                data.uvMin = { glyph->uvMinX, glyph->uvMinY };
                data.uvMax = { glyph->uvMaxX, glyph->uvMaxY };
                data.entityID = static_cast<uint32_t>(entity);
                
                result_.charInstances.push_back(data);

                // 次の文字へ
                xOffset += glyph->xAdvance * (renderer.size / 32.0f);
            }
        });

        if (entityCount > 0) {
            static uint32_t logCount = 0;
            if (logCount++ % 100 == 0) {
                Engine::Console::Log(std::format("[SystemLog] TextSystem found {} entities.", entityCount));
            }
        }
    }

    const TextResult& GetResult() const { return result_; }
    void Reset() { result_.charInstances.clear(); }

private:
    TextResult result_;
};

} // namespace Engine::ECS
