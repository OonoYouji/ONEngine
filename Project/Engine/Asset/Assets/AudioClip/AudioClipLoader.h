#pragma once

#include "../IAssetLoader.h"
#include "AudioClip.h"

namespace ONEngine {

template<>
class AssetLoader<AudioClip> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetLoader() = default;
	~AssetLoader() override = default;

	std::optional<AudioClip> Load(const std::string& _filepath);
	std::optional<AudioClip> Reload(const std::string& _filepath, AudioClip* _src = nullptr);

};

} /// namespace ONEngine