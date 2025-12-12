#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Asset/Assets/IAsset.h"
#include "Engine/Asset/Assets/IAssetLoader.h"
#include "Engine/Asset/Collection/Container/AssetContainer.h"

namespace ONEngine {

class IAssetBundle {
public:
	virtual ~IAssetBundle() = default;

	virtual void Load(const std::string& _filepath) = 0;
	virtual void Reload(const std::string& _filepath) = 0;
	virtual const Guid& GetGuid(const std::string& _filepath) const = 0;
	virtual void Remove(const std::string& _filepath) = 0;
	virtual bool Contains(const Guid& _guid) const = 0;
	virtual bool Contains(const std::string& _filepath) const = 0;
};

template <IsAsset T>
class AssetBundle : public IAssetBundle {
public:

	AssetBundle() = default;
	~AssetBundle() override = default;

	std::unique_ptr<AssetLoader<T>> loader;
	std::unique_ptr<AssetContainer<T>> container;

	void Load(const std::string& _filepath) override {
		if (container->GetIndex(_filepath) == -1) {
			auto texture = loader->Load(_filepath);
			if (texture.has_value()) {
				container->Add(_filepath, std::move(texture.value()));
			}
		}
	}

	void Reload(const std::string& _filepath) override {
		int32_t index = container->GetIndex(_filepath);
		if (index != -1) {
			T* src = container->Get(index);
			auto reloadedAsset = loader->Reload(_filepath, src);
			if (reloadedAsset.has_value()) {
				container->Remove(index);
				container->Add(_filepath, std::move(reloadedAsset.value()));
			}
		}
	}


	const Guid& GetGuid(const std::string& _filepath) const override {
		return container->GetGuid(_filepath);
	}

	void Remove(const std::string& _filepath) override {
		container->Remove(_filepath);
	}

	bool Contains(const Guid& _guid) const override {
		return container->GetIndex(_guid) != -1;
	}

	bool Contains(const std::string& _filepath) const override {
		return container->GetIndex(_filepath) != -1;
	}

};

} /// namespace ONEngine
