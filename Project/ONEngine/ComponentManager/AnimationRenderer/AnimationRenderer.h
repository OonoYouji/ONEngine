#pragma once

/// std
#include <unordered_map>
#include <string>
#include <memory>
#include <list>

/// math
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

/// engine
#include "GraphicManager/PipelineState/PipelineState.h"
	
/// base class & component
#include "../Base/BaseComponent.h"
#include "../Transform/Transform.h"

/// 
#include "Skeleton.h"
#include "Skinning.h"
#include <minwindef.h>


enum ANIMATION_FLAG{
	ANIMATION_FLAG_NONE,   /// 空
	ANIMATION_FLAG_NOLOOP, /// リピート再生しない
};



/// ===================================================
/// アニメーションの
/// ===================================================
class AnimationRenderer final : public BaseComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AnimationRenderer(const std::string& modelFilePath);
	~AnimationRenderer();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

	void DrawCall();

	void SetModel(const std::string& filePath);

	void LoadAnimation(const std::string& filePath);
	void ChangeAnimation(const std::string& _filePath);


	Vec3       CalculateValue(const std::vector<KeyframeVec3>& keyframe, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

	void ApplyAnimation(Skeleton& skeleton);

	void SkinClusterUpdate(SkinCluster& _skinCluster, const Skeleton& _skeleton) const;


	float GetDuration(const std::string& _filePath) const { return durationMap_.at(_filePath); }

	void SetTotalTime(float _totalTime, const std::string& _filePath);

	void SetTimeRate(float _timeRate);

	void SetAnimationFlags(int _flags);

	void SetTime(float _time);

	void Restart();

	const std::string& GetCurrentNodeAnimationKey() const {
		return currentNodeAnimationKey_;
	}

	Skeleton* GetSkeleton() {
		return &skeletonMap_.at(currentNodeAnimationKey_);
	}

	std::vector<Material>& GetMaterials() const { return pModel_->GetMaterials(); }

	
	void SetIsStopAnimation(bool _isStop);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isStopAnimation_ = false;

	float animationTime_;
	float timeRate_ = 1.0f;
	int animationFlags_ = 0;

	bool isPlayedOnce_ = false;

	Transform transform_;

	//std::unordered_map<std::string, NodeAnimation> nodeAnimationArray_;

	using NodeAnimationMap = std::unordered_map<std::string, NodeAnimation>;
	std::unordered_map<std::string, NodeAnimationMap> multiNodeAnimationArray_;
	std::string currentNodeAnimationKey_;

	std::unordered_map<std::string, float> durationMap_;


	class Model* pModel_;

	std::unordered_map<std::string, SkinCluster> skinClusterMap_;
	std::unordered_map<std::string, Skeleton>    skeletonMap_;

};


class AnimationRendererCommon final {
public:

	static AnimationRendererCommon* GetInstance() {
		static AnimationRendererCommon instance;
		return &instance;
	}

	void Initialize();
	void Finalize();

	void PreDraw();
	void PostDraw();

	void AddAnimationRenderer(AnimationRenderer* _animationRenderer);

	void BindDirectionalLightToCommandList(UINT _rootParameter, ID3D12GraphicsCommandList* _commandList);

	void SetDirectionalLight(class DirectionalLight* _directionalLight);

private:

	std::unique_ptr<PipelineState> pipelineState_ = nullptr;
	PipelineState::Shader          shader_        = {};

	D3D12_VERTEX_BUFFER_VIEW vbv_;

	std::list<AnimationRenderer*> actives_;

	class DirectionalLight* pDirectionalLight_ = nullptr;

};
