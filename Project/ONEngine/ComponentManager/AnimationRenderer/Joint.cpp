#include "Joint.h"

Joint::Joint() {
	transform.rotateOrder = QUATERNION;
}

int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;

	joint.name                   = node.name;
	joint.transform.matTransform = node.transform.matTransform;
	joint.matSkeletonSpace       = Mat4::kIdentity;
	joint.transform              = node.transform;
	joint.index                  = static_cast<int32_t>(joints.size());
	joint.parent                 = parent;
	joints.push_back(joint);

	for(const Node& child : node.children) {
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].chidren.push_back(childIndex);
	}

	return joint.index;
}
