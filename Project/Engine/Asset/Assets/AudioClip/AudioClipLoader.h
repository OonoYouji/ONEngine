#pragma once

#include "../IAssetLoader.h"
#include "../../Meta/MetaFile.h"
#include "AudioClip.h"

namespace ONEngine::Asset {

template<>
class AssetLoader<AudioClip, AudioClipMeta> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetLoader() = default;
	~AssetLoader() override = default;

	std::optional<AudioClip> Load(const std::string& _filepath, Meta<AudioClipMeta> meta);
	std::optional<AudioClip> Reload(const std::string& _filepath, AudioClip* _src = nullptr, Meta<AudioClipMeta> meta = {});

	Meta<AudioClipMeta> GetMetaData(const std::string& _filepath);

};

} /// namespace ONEngine::Asset