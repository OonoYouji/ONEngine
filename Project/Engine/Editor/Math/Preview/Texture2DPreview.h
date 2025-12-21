#pragma once

/// engine
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Core/Utility/Utility.h"

namespace Editor {

void ShowTexture2DPreview(const std::string& _name, ONEngine::Texture* _texture, const ONEngine::Vector2& _textureSize, float _previewFactor);

} /// namespace Editor
