#include "Skeleton.h"


void Skeleton::Update() {

	for(Joint& joint : joints) {
		joint.transform.Update();

		joint.matSkeletonSpace = joint.transform.matTransform;
		if(joint.parent) {
			joint.matSkeletonSpace *= joints[*joint.parent].matSkeletonSpace;
		}
	}
}


Skeleton CreateSkeleton(const Node& rootNode) {
	Skeleton result;
	result.root = CreateJoint(rootNode, {}, result.joints);

	for(const Joint& joint : result.joints) {
		result.jointMap.emplace(joint.name, joint.index);
	}

	result.Update();
	return result;
}
