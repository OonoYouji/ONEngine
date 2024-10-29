#include "SkeletonRenderer.h"

/// component
#include "../MeshInstancingRenderer/MeshInstancingRenderer.h"

/// lib
#include "Debug/Assert.h"


void SkeletonRenderer::Initialize() {
	/// jointの数分だけ生成
	meshInstancingRenderer_.reset(new MeshInstancingRenderer(128));
	meshInstancingRenderer_->Initialize();
	meshInstancingRenderer_->SetModel("Joint");

}

void SkeletonRenderer::Update() {
	Assert(pSkeleton_, "skeleton == nullptr !!!");


	meshInstancingRenderer_->ResetTransformArray();
	for(Joint& joint : pSkeleton_->joints) {
		meshInstancingRenderer_->AddTransform(&joint.transform);
	}

}

void SkeletonRenderer::Draw() {
	meshInstancingRenderer_->Draw();
}

void SkeletonRenderer::SetSkeleton(Skeleton* _skeleton) {
	pSkeleton_ = _skeleton;
}
