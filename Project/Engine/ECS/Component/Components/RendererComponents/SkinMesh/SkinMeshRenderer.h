#pragma once

/// std
#include <string>
#include <optional>
#include <unordered_map>

/// external
#include <mono/jit/jit.h>
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Mesh/Skinning.h"

/// /////////////////////////////////////////////
/// スキニングアニメーションのRender Component
/// /////////////////////////////////////////////
namespace ONEngine {

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

	bool isPlaying_;
	float animationTime_;
	float duration_;
	float animationScale_;

	std::unordered_map<std::string, NodeAnimation> nodeAnimationMap_;
	std::optional<SkinCluster> skinCluster_; ///< スキンアニメーションのデータ
	Skeleton skeleton_; ///< ボーンデータ
	bool isChangingMesh_;

public:
	/// ==========================================
	/// public : accessors
	/// ==========================================

	void SetMeshPath(const std::string& _path);
	void SetTexturePath(const std::string& _path);

	void SetColor(const Vector4& _color);

	void SetIsPlaying(bool _isPlaying);
	void SetAnimationTime(float _time);
	void SetDuration(float _duration);
	void SetAnimationScale(float _scale);


	const std::string& GetMeshPath() const;
	const std::string& GetTexturePath() const;

	const Vector4& GetColor() const;

	bool GetIsPlaying() const;
	float GetAnimationTime() const;
	float GetDuration() const;
	float GetAnimationScale() const;

	const Skeleton& GetSkeleton() const;

};


namespace ComponentDebug {
	void SkinMeshRendererDebug(SkinMeshRenderer* _smr);
}


/// ===================================================
/// csで使用するための関数群
/// ===================================================

SkinMeshRenderer* GetSkinMeshRenderer(uint64_t _nativeHandle);

MonoString* InternalGetMeshPath(uint64_t _nativeHandle);
void InternalSetMeshPath(uint64_t _nativeHandle, MonoString* _path);

MonoString* InternalGetTexturePath(uint64_t _nativeHandle);
void InternalSetTexturePath(uint64_t _nativeHandle, MonoString* _path);

bool InternalGetIsPlaying(uint64_t _nativeHandle);
void InternalSetIsPlaying(uint64_t _nativeHandle, bool _isPlaying);

float InternalGetAnimationTime(uint64_t _nativeHandle);
void InternalSetAnimationTime(uint64_t _nativeHandle, float _time);

float InternalGetAnimationScale(uint64_t _nativeHandle);
void InternalSetAnimationScale(uint64_t _nativeHandle, float _scale);

void InternalGetJointTransform(uint64_t _nativeHandle, MonoString* _jointName, Vector3* _outScale, Quaternion* _outRotation, Vector3* _outPosition);



void from_json(const nlohmann::json& _j, SkinMeshRenderer& _smr);
void to_json(nlohmann::json& _j, const SkinMeshRenderer& _smr);

} /// ONEngine