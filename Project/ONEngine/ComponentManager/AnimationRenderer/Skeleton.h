#pragma once

/// std
#include <unordered_map>

/// other
#include "Joint.h"
#include "GraphicManager/ModelManager/Model.h"

struct Skeleton {

	void Update();

public:

	int32_t root;
	std::unordered_map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};


Skeleton CreateSkeleton(const Node& rootNode);