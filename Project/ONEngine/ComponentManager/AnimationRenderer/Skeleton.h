#pragma once

/// std
#include <unordered_map>

/// other
#include "Joint.h"
#include "GraphicManager/ModelManager/Model.h"


template <typename T>
struct Keyframe {
	float time;
	T value;
};

using KeyframeVec3 = Keyframe<Vec3>;
using KeyframeQuaternion = Keyframe<Quaternion>;


struct NodeAnimation {
	std::vector<KeyframeVec3>       translate;
	std::vector<KeyframeQuaternion> rotate;
	std::vector<KeyframeVec3>       scale;
};

Vec3       CalculateValue(const std::vector<KeyframeVec3>& keyframe, float time);
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);


struct Skeleton {

	void Update(
		float animationTime,
		std::unordered_map<std::string, NodeAnimation>& _nodeAnimationArray
	);

public:

	int32_t root;
	std::unordered_map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};


Skeleton CreateSkeleton(const Node& rootNode);