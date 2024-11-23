#include "Skeleton.h"

/// engine
#include "FrameManager/Time.h"

/// lib
#include "Debugger/Assertion.h"

#include "AnimationRenderer.h"


void Skeleton::Update(float animationTime, std::unordered_map<std::string, NodeAnimation>& _nodeAnimationArray) {

	for(Joint& joint : joints) {

		joint.animationTime = animationTime;

		NodeAnimation& rootAnimation = _nodeAnimationArray[joint.name];

		if(!rootAnimation.translate.empty()) { joint.transform.position   = CalculateValue(rootAnimation.translate, joint.animationTime); }
		if(!rootAnimation.rotate.empty())    { joint.transform.quaternion = CalculateValue(rootAnimation.rotate, joint.animationTime); }
		if(!rootAnimation.scale.empty())     { joint.transform.scale      = CalculateValue(rootAnimation.scale, joint.animationTime); }
		joint.transform.Update();
		joint.offsetTransform.Update();

		joint.transform.matTransform *= joint.offsetTransform.matTransform;

		joint.matSkeletonSpace = joint.transform.matTransform;
		if(joint.parent) {
			joint.matSkeletonSpace *= joints[*joint.parent].matSkeletonSpace;
		}
	}
}



Vec3 CalculateValue(const std::vector<KeyframeVec3>& keyframeArray, float time) {
	Assert(!keyframeArray.empty(), "keyframe empty...");

	if(keyframeArray.size() == 1 || time <= keyframeArray[0].time) {
		return keyframeArray[0].value;
	}

	for(size_t index = 0; index < keyframeArray.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if(keyframeArray[index].time <= time && time <= keyframeArray[nextIndex].time) {
			float t = (time - keyframeArray[index].time) / (keyframeArray[nextIndex].time - keyframeArray[index].time);
			return Vec3::Lerp(keyframeArray[index].value, keyframeArray[nextIndex].value, t);
		}
	}

	return keyframeArray.back().value;
}

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframeArray, float time) {
	Assert(!keyframeArray.empty(), "keyframe empty...");

	if(keyframeArray.size() == 1 || time <= keyframeArray[0].time) {
		return keyframeArray[0].value;
	}

	for(size_t index = 0; index < keyframeArray.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if(keyframeArray[index].time <= time && time <= keyframeArray[nextIndex].time) {
			float t = (time - keyframeArray[index].time) / (keyframeArray[nextIndex].time - keyframeArray[index].time);
			return Quaternion::Slerp(keyframeArray[index].value, keyframeArray[nextIndex].value, t);
		}
	}

	return keyframeArray.back().value;
}


Skeleton CreateSkeleton(const Node& rootNode) {
	Skeleton result;
	result.root = CreateJoint(rootNode, {}, result.joints);

	for(const Joint& joint : result.joints) {
		result.jointMap.emplace(joint.name, joint.index);
	}

	//result.Update();
	return result;
}

