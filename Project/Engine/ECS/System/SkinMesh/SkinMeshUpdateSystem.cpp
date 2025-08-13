#include "SkinMeshUpdateSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

SkinMeshUpdateSystem::SkinMeshUpdateSystem(DxManager* _dxManager, GraphicsResourceCollection* _resourceCollection)
	: pDxManager_(_dxManager), pResourceCollection_(_resourceCollection) {}

void SkinMeshUpdateSystem::RuntimeUpdate([[maybe_unused]] EntityComponentSystem* _ecs, const std::vector<class GameEntity*>& _entities) {

	std::vector<SkinMeshRenderer*> skinMeshRenderers;
	for (auto& entity : _entities) {
		SkinMeshRenderer* skinMesh = entity->GetComponent<SkinMeshRenderer>();
		if (skinMesh && skinMesh->enable) {
			skinMeshRenderers.push_back(skinMesh);
		}
	}

	if (skinMeshRenderers.empty()) {
		return; ///< 描画するスキンメッシュがない場合は何もしない
	}


	for (auto& skinMesh : skinMeshRenderers) {

		/// skin clusterが存在しないなら生成する
		if (skinMesh->isChangingMesh_) {
			Model* model = pResourceCollection_->GetModel(skinMesh->GetMeshPath());
			if (!model) {
				//!< nullの場合は適当なメッセージを出力してスキップ
				Console::Log("[error] SkinMeshUpdateSystem::Update: Model not found for path: " + skinMesh->GetMeshPath());
				skinMesh->isChangingMesh_ = false; ///< モデルが見つからない場合はメッシュ変更フラグを下げる
				continue; ///< モデルが見つからない場合はスキップ
			}


			Skeleton skeleton = ANIME_MATH::CreateSkeleton(model->GetRootNode());
			SkinCluster skinCluster = ANIME_MATH::CreateSkinCluster(skeleton, model, pDxManager_);

			skinMesh->skinCluster_ = std::move(skinCluster);
			skinMesh->skeleton_ = std::move(skeleton);

			skinMesh->animationTime_ = 0.0f;
			skinMesh->duration_ = model->GetAnimationDuration();

			skinMesh->nodeAnimationMap_ = model->GetNodeAnimationMap();

			skinMesh->isChangingMesh_ = false;

			UpdateSkeleton(skinMesh);
			UpdateSkinCluster(skinMesh);
		}


		/// skin clusterがあるかチェック
		if (!skinMesh->skinCluster_) {
			continue;
		}

		if (!skinMesh->isPlaying_) {
			continue;
		}


		skinMesh->animationTime_ += Time::DeltaTime();
		skinMesh->animationTime_ = std::fmod(skinMesh->animationTime_, skinMesh->GetDuration());

		UpdateSkeleton(skinMesh);
		UpdateSkinCluster(skinMesh);

	}


}

void SkinMeshUpdateSystem::UpdateSkeleton(SkinMeshRenderer* _smr) {
	Skeleton& skeleton = _smr->skeleton_;

	/// ------------------------------------
	/// スケルトンの更新
	/// ------------------------------------
	for (Joint& joint : skeleton.joints) {

		NodeAnimation& rootAnimation = _smr->nodeAnimationMap_[joint.name];

		if (!rootAnimation.translate.empty()) { joint.transform.position = ANIME_MATH::CalculateValue(rootAnimation.translate, _smr->animationTime_); }
		if (!rootAnimation.rotate.empty()) { joint.transform.rotate = ANIME_MATH::CalculateValue(rootAnimation.rotate, _smr->animationTime_); }
		if (!rootAnimation.scale.empty()) { joint.transform.scale = ANIME_MATH::CalculateValue(rootAnimation.scale, _smr->animationTime_); }
		joint.transform.Update();

		joint.matSkeletonSpace = joint.transform.matWorld;
		if (joint.parent) {
			joint.matSkeletonSpace *= skeleton.joints[*joint.parent].matSkeletonSpace;
		}

		joint.matWorld = joint.matSkeletonSpace * _smr->GetOwner()->GetTransform()->matWorld;
	}
}

void SkinMeshUpdateSystem::UpdateSkinCluster(SkinMeshRenderer* _smr) {
	Skeleton& skeleton = _smr->skeleton_;
	SkinCluster& skinCluster = _smr->skinCluster_.value();


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
