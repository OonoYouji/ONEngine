#pragma once

/// std
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

/// component
#include "ComponentManager/Transform/Transform.h"

/// engine
#include "GraphicManager/ModelManager/Model.h"

struct Joint {
	Joint();

	Transform              transform;
	Mat4                   matSkeletonSpace;
	std::string            name;
	std::vector<int32_t>   chidren;
	int32_t                index;
	std::optional<int32_t> parent;

	float animationTime;
};


int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);