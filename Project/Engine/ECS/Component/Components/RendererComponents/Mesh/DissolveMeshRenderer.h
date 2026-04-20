#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

namespace ONEngine {
class DissolveMeshRenderer;
}

namespace ONEngine::Asset {
class AssetCollection;
}


namespace ONEngine {

/// ///////////////////////////////////////////////////
/// ディゾルブの比較方法
/// ///////////////////////////////////////////////////
enum class DissolveCompare {
	LessEqual,
	GreaterEqual
};

/// ///////////////////////////////////////////////////
/// メッシュをディゾルブ表現で表示するためのコンポーネント
/// ///////////////////////////////////////////////////
class DissolveMeshRenderer : public IRenderComponent {
	friend void ShowGUI(DissolveMeshRenderer* _dmr, Asset::AssetCollection* _ac);
	friend void from_json(const nlohmann::json& _j, DissolveMeshRenderer& _dmr);
	friend void to_json(nlohmann::json& _j, const DissolveMeshRenderer& _dmr);
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	DissolveMeshRenderer();
	~DissolveMeshRenderer();

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	Guid meshGuid_;
	Asset::Material material_;
	Guid dissolveTexture_;

	float dissolveThreshold_ = 0.5f;
	
	DissolveCompare dissolveCompare_ = DissolveCompare::LessEqual;

public:
	/// ===========================================
	/// public : accessors
	/// ===========================================

	const Guid& GetMeshGuid() const;
	const Guid& GetDissolveTextureGuid() const;

	uint32_t GetDissolveTextureId(Asset::AssetCollection* _ac) const;
	float GetDissolveThreshold() const;

	GPUMaterial GetGPUMaterial(Asset::AssetCollection* _ac) const;

	uint32_t GetDissolveCompare() const;
};

} /// namespace ONEngine