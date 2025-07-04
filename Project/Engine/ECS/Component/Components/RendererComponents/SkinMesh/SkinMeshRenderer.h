#pragma once

/// std
#include <string>
#include <optional>
#include <unordered_map>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Resource/ResourceData/Skinning.h"

/// /////////////////////////////////////////////
/// スキニングアニメーションのRender Component
/// /////////////////////////////////////////////
class SkinMeshRenderer : public IRenderComponent {
	friend class SkinMeshUpdateSystem;
	friend class SkinMeshRenderingPipeline;
public:
	/// =========================================
	/// public : methods
	/// =========================================

	SkinMeshRenderer();
	~SkinMeshRenderer() override = default;

private:
	/// =========================================
	/// private : methods
	/// =========================================

	std::string meshPath_;
	std::string texturePath_;
	Vector4 color_;

	float animationTime_;
	float duration_;

	std::unordered_map<std::string, NodeAnimation> nodeAnimationMap_;
	SkinCluster skinCluster_; ///< スキンアニメーションのデータ
	Skeleton skeleton_; ///< ボーンデータ
	bool isChangingMesh_;

public:
	/// ==========================================
	/// public : accessors
	/// ==========================================

	void SetMeshPath(const std::string& _path);
	void SetTexturePath(const std::string& _path);

	void SetColor(const Vector4& _color);

	void SetAnimationTime(float _time);
	void SetDuration(float _duration);


	const std::string& GetMeshPath() const;
	const std::string& GetTexturePath() const;

	const Vector4& GetColor() const;

	float GetAnimationTime() const;
	float GetDuration() const;
};


namespace COMP_DEBUG {
	void SkinMeshRendererDebug(SkinMeshRenderer* _smr);
}

void from_json(const nlohmann::json& _j, SkinMeshRenderer& _smr);
void to_json(nlohmann::json& _j, const SkinMeshRenderer& _smr);
