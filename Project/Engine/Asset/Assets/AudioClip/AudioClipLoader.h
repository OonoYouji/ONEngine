#pragma once

#include "../IAssetLoader.h"
#include "AudioClip.h"

namespace ONEngine {

template<>
class AssetLoaderT<AudioClip> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetLoaderT() = default;
	~AssetLoaderT() override = default;

	AudioClip Load(const std::string& _filepath);
	AudioClip Reload(const std::string& _filepath, AudioClip* _src = nullptr);

};

} /// namespace ONEngine