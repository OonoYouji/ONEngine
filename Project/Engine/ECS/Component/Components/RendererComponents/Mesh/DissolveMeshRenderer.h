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
	friend class AnimationPlayer;
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

	float dissolveThreshold_ = 1.0f;
	
	DissolveCompare dissolveCompare_ = DissolveCompare::LessEqual;

	float edgeWidth_ = 0.05f;
	Vector4 edgeColor_ = { 1.0f, 0.5f, 0.0f, 1.0f }; // Orange glow default

	friend class AnimationPlayer;

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

	float GetEdgeWidth() const { return edgeWidth_; }
	const Vector4& GetEdgeColor() const { return edgeColor_; }


	void SetThreshold(float threshold) {
		dissolveThreshold_ = threshold;
	}

	void SetUVTransform(const UVTransform& _uvTransform) {
		material_.uvTransform = _uvTransform;
	}

	const UVTransform& GetUVTransform() const {
		return material_.uvTransform;
	}

	/// @brief アニメーション制御用マテリアルへの参照取得
	Asset::Material& GetMaterialForAnimation() { return material_; }
	
	/// @brief アニメーション制御用しきい値への参照取得
	float& GetThresholdForAnimation() { return dissolveThreshold_; }

	/// @brief アニメーション制御用エッジ幅への参照取得
	float& GetEdgeWidthForAnimation() { return edgeWidth_; }

	/// @brief アニメーション制御用エッジ色への参照取得
	Vector4& GetEdgeColorForAnimation() { return edgeColor_; }

};

} /// namespace ONEngine