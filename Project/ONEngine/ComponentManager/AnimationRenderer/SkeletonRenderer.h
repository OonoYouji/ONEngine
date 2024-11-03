#pragma once

/// std
#include <memory>

/// base class
#include "../Base/BaseComponent.h"

#include "Skeleton.h"


class SkeletonRenderer : public BaseComponent {
public:

	SkeletonRenderer() {}
	~SkeletonRenderer() {}

	void Initialize() override;
	void Update()     override;
	void Draw()       override;

	void SetSkeleton(Skeleton* _skeleton);


private:
	std::unique_ptr<class MeshInstancingRenderer> meshInstancingRenderer_ = nullptr;
	Skeleton* pSkeleton_ = nullptr;
};