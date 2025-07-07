#include "SkinMeshUpdateSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

SkinMeshUpdateSystem::SkinMeshUpdateSystem(DxManager* _dxManager, GraphicsResourceCollection* _resourceCollection)
	: pDxManager_(_dxManager), pResourceCollection_(_resourceCollection) {}

void SkinMeshUpdateSystem::Update(EntityComponentSystem* _ecs) {

	ComponentArray<SkinMeshRenderer>* skinMeshArray = _ecs->GetComponentArray<SkinMeshRenderer>();
	if (!skinMeshArray) {
		return; ///< スキンメッシュのコンポーネントが存在しない場合は何もしない
	}

	for (auto& skinMesh : skinMeshArray->GetUsedComponents()) {
		if (!skinMesh || !skinMesh->enable) {
			continue; ///< スキンメッシュが無効な場合はスキップ
		}

		/// skin clusterが存在しないなら生成する
		if (skinMesh->isChangingMesh_) {
			Model* model = pResourceCollection_->GetModel(skinMesh->GetMeshPath());
			Skeleton skeleton = ANIME_MATH::CreateSkeleton(model->GetRootNode());
			SkinCluster skinCluster = ANIME_MATH::CreateSkinCluster(skeleton, model, pDxManager_);

			skinMesh->skinCluster_ = std::move(skinCluster);
			skinMesh->skeleton_ = std::move(skeleton);

			skinMesh->animationTime_ = 0.0f;
			skinMesh->duration_ = model->GetAnimationDuration();
		
			skinMesh->nodeAnimationMap_ = model->GetNodeAnimationMap();
			
			skinMesh->isChangingMesh_ = false;
		}


		/// skin clusterがあるかチェック
		if (!skinMesh->skinCluster_) {
			continue;
		}


		skinMesh->animationTime_ += Time::DeltaTime();
		skinMesh->animationTime_ = std::fmod(skinMesh->animationTime_, skinMesh->GetDuration());

		Skeleton& skeleton = skinMesh->skeleton_;
		SkinCluster& skinCluster = skinMesh->skinCluster_.value();


		/// ------------------------------------
		/// スケルトンの更新
		/// ------------------------------------
		for (Joint& joint : skeleton.joints) {

			NodeAnimation& rootAnimation = skinMesh->nodeAnimationMap_[joint.name];

			if (!rootAnimation.translate.empty()) { joint.transform.position = ANIME_MATH::CalculateValue(rootAnimation.translate, skinMesh->animationTime_); }
			if (!rootAnimation.rotate.empty()) { joint.transform.rotate = ANIME_MATH::CalculateValue(rootAnimation.rotate, skinMesh->animationTime_); }
			if (!rootAnimation.scale.empty()) { joint.transform.scale = ANIME_MATH::CalculateValue(rootAnimation.scale, skinMesh->animationTime_); }
			joint.transform.Update();

			joint.matSkeletonSpace = joint.transform.matWorld;
			if (joint.parent) {
				joint.matSkeletonSpace *= skeleton.joints[*joint.parent].matSkeletonSpace;
			}

			Vector3 jointPosition = Matrix4x4::Transform(Vector3::kZero, joint.matSkeletonSpace * skinMesh->GetOwner()->GetTransform()->matWorld);
			Gizmo::DrawWireSphere(jointPosition, 0.3f, Color::kRed);

		}


		/// ------------------------------------
		/// スキンクラスターの更新
		/// ------------------------------------
		for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {

			if (jointIndex >= skinCluster.matBindPoseInverseArray.size()) {
				Console::Log("[warring] SkinMeshUpdateSystem::Update: jointIndex out of range for matBindPoseInverseArray");
				continue; ///< 範囲外の場合はスキップ
			}

			skinCluster.mappedPalette[jointIndex].matSkeletonSpace =
				skinCluster.matBindPoseInverseArray[jointIndex] * skeleton.joints[jointIndex].matSkeletonSpace;

			skinCluster.mappedPalette[jointIndex].matSkeletonSpaceInverseTranspose =
				Matrix4x4::MakeTranspose(Matrix4x4::MakeInverse(skinCluster.mappedPalette[jointIndex].matSkeletonSpace));

		}



	}


}
