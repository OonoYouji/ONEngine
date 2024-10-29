#pragma once

/// std
#include <unordered_map>
#include <string>

/// math
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

/// base class & component
#include "../Base/BaseComponent.h"
#include "../Transform/Transform.h"

/// 
#include "Skeleton.h"


/// ===================================================
/// アニメーションの
/// ===================================================
class AnimationRenderer final : public BaseComponent {

	template <typename T>
	struct Keyframe {
		float time;
		T value;
	};

	using KeyframeVec3       = Keyframe<Vec3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;


	struct NodeAnimation {
		std::vector<KeyframeVec3>       translate;
		std::vector<KeyframeQuaternion> rotate;
		std::vector<KeyframeVec3>       scale;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AnimationRenderer();
	~AnimationRenderer();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;

	void SetModel(const std::string& filePath);

	void LoadAnimation(const std::string& filePath);

	Vec3       CalculateValue(const std::vector<KeyframeVec3>& keyframe, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

	void ApplyAnimation(Skeleton& skeleton);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	float duration_;
	float animationTime_;

	Transform transform_;

	std::unordered_map<std::string, NodeAnimation> nodeAnimationArray_;

	class Model* pModel_;

};


class AnimationRendererCommon final {
public:

	void PreDraw();
	void PostDraw();

private:


};
