#include "SkinMeshUpdateSystem.h"

using namespace ONEngine;

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/Asset/Collection/AssetCollection.h"

SkinMeshUpdateSystem::SkinMeshUpdateSystem(DxManager* _dxm, Asset::AssetCollection* _assetCollection)
	: pDxManager_(_dxm), pAssetCollection_(_assetCollection) {
}

void SkinMeshUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {

	/// Compの配列を取得＆使用中のCompがなければ終了
	ComponentArray<SkinMeshRenderer>* skinMeshArray = _ecs->GetComponentArray<SkinMeshRenderer>();
	if (!skinMeshArray || skinMeshArray->GetUsedComponents().empty()) {
		return;
	}

	for (auto& skinMesh : skinMeshArray->GetUsedComponents()) {
		/// 以降の処理を無視する条件
		if (!skinMesh || !skinMesh->enable || !skinMesh->GetOwner()->active) {
			continue;
		}

		/// skin clusterが存在しないなら生成する
		if (skinMesh->isChangingMesh_) {
			Asset::Model* model = pAssetCollection_->GetModel(skinMesh->GetMeshPath());
			if (!model) {
				continue; ///< モデルが見つからない場合はスキップ（isChangingMesh_はtrueのままにする）
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

		/// アニメーションが再生されていないならスキップ
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
	/// ------------------------------------
	/// スケルトンの更新
	/// ------------------------------------
	UpdateSkeletonRecursive(_smr, _smr->skeleton_.root, std::nullopt);
}

void SkinMeshUpdateSystem::UpdateSkeletonRecursive(SkinMeshRenderer* _smr, int32_t _jointIndex, const std::optional<int32_t>& _parentIndex) {
	Skeleton& skeleton = _smr->skeleton_;
	Joint& joint = skeleton.joints[_jointIndex];

	/// アニメーションの適用
	auto it = _smr->nodeAnimationMap_.find(joint.name);
	if (it != _smr->nodeAnimationMap_.end()) {
		NodeAnimation& animation = it->second;
		if (!animation.translate.empty()) { joint.transform.position = ANIME_MATH::CalculateValue(animation.translate, _smr->animationTime_); }
		if (!animation.rotate.empty()) { joint.transform.rotate = ANIME_MATH::CalculateValue(animation.rotate, _smr->animationTime_); }
		if (!animation.scale.empty()) { joint.transform.scale = ANIME_MATH::CalculateValue(animation.scale, _smr->animationTime_); }
	}
	joint.transform.Update();

	/// スケルトン空間行列の計算 (Local * Parent)
	if (_parentIndex) {
		joint.matSkeletonSpace = joint.transform.matWorld * skeleton.joints[*_parentIndex].matSkeletonSpace;
	} else {
		joint.matSkeletonSpace = joint.transform.matWorld;
	}

	/// ワールド行列の計算
	joint.matWorld = joint.matSkeletonSpace * _smr->GetOwner()->GetTransform()->matWorld;

	/// 子の更新
	for (int32_t childIndex : joint.children) {
		UpdateSkeletonRecursive(_smr, childIndex, _jointIndex);
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

		/// スキニング行列 = 逆バインドポーズ行列 * スケルトン空間行列
		skinCluster.mappedPalette[jointIndex].matSkeletonSpace =
			skinCluster.matBindPoseInverseArray[jointIndex] * skeleton.joints[jointIndex].matSkeletonSpace;

		skinCluster.mappedPalette[jointIndex].matSkeletonSpaceInverseTranspose =
			Matrix4x4::MakeTranspose(Matrix4x4::MakeInverse(skinCluster.mappedPalette[jointIndex].matSkeletonSpace));

	}

}
