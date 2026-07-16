#include "SkinMeshUpdateSystem.h"

using namespace ONEngine;

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/Asset/Collection/AssetCollection.h"

/**
 * @brief コンストラクタ
 */
SkinMeshUpdateSystem::SkinMeshUpdateSystem(DxManager* _dxm, Asset::AssetCollection* _assetCollection)
	: pDxManager_(_dxm), pAssetCollection_(_assetCollection) {
}

/**
 * @brief 毎フレームのスキンメッシュ更新処理を実行します。
 */
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
			Console::LogInfo(std::format("SkinMeshUpdateSystem: Changing mesh for entity '{}' to path: '{}'", 
				skinMesh->GetOwner()->GetName(), skinMesh->GetMeshPath()));

			Asset::Model* model = pAssetCollection_->GetModel(skinMesh->GetMeshPath());
			if (!model) {
				static std::unordered_map<std::string, bool> loggedMissing;
				if (!loggedMissing[skinMesh->GetMeshPath()]) {
					Console::LogError(std::format("SkinMeshUpdateSystem: Failed to find model at path: '{}'. Registered models may not include this path.", skinMesh->GetMeshPath()));
					loggedMissing[skinMesh->GetMeshPath()] = true;
				}
				continue; 
			}

			Console::LogInfo(std::format("SkinMeshUpdateSystem: Found model '{}'. Creating Skeleton and SkinCluster...", skinMesh->GetMeshPath()));
			Skeleton skeleton = ANIME_MATH::CreateSkeleton(model->GetRootNode());
			SkinCluster skinCluster = ANIME_MATH::CreateSkinCluster(skeleton, model, pDxManager_);

			skinMesh->skinCluster_ = std::move(skinCluster);
			skinMesh->skeleton_ = std::move(skeleton);
			Console::LogInfo(std::format("SkinMeshUpdateSystem: Skeleton created with {} joints.", skinMesh->skeleton_.joints.size()));

			skinMesh->animationTime_ = 0.0f;
			if (!model->GetAnimationClips().empty()) {
				const auto& clips = model->GetAnimationClips();
				const auto& clip = clips.begin()->second;
				skinMesh->duration_ = clip.duration;
				skinMesh->nodeAnimationMap_ = clip.nodeAnimationMap;
				Console::LogInfo(std::format("SkinMeshUpdateSystem: Initialized with clip '{}' (duration: {:.2f}s). Total clips available: {}", clip.name, clip.duration, clips.size()));
				
				// List all clips for debugging
				for(const auto& [hash, c] : clips) {
					Console::LogInfo(std::format("  Available Clip: '{}' (hash: {})", c.name, hash));
				}
			} else {
				skinMesh->duration_ = 0.0f;
				skinMesh->nodeAnimationMap_.clear();
				Console::LogWarning(std::format("SkinMeshUpdateSystem: Model '{}' has no animation clips.", skinMesh->GetMeshPath()));
			}

			skinMesh->isChangingMesh_ = false;

			UpdateSkeleton(skinMesh);
			UpdateSkinCluster(skinMesh);
			Console::LogInfo("SkinMeshUpdateSystem: Initialization complete.");
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

/**
 * @brief スキンメッシュ内の全ジョイント（関節）の姿勢から、各ボーンのワールド空間行列（スケルトン行列）を計算し更新します。
 */
void SkinMeshUpdateSystem::UpdateSkeleton(SkinMeshRenderer* _smr) {
	/// ------------------------------------
	/// スケルトンの更新
	/// ------------------------------------
	UpdateSkeletonRecursive(_smr, _smr->skeleton_.root, std::nullopt);
}

/**
 * @brief ボーン階層構造の親子関係に従い、再帰的にワールド空間のジョイント行列を更新します。
 */
void SkinMeshUpdateSystem::UpdateSkeletonRecursive(SkinMeshRenderer* _smr, int32_t _jointIndex, const std::optional<int32_t>& _parentIndex) {
	Skeleton& skeleton = _smr->skeleton_;
	Joint& joint = skeleton.joints[_jointIndex];

	/// アニメーションの適用
	auto it = _smr->nodeAnimationMap_.find(joint.nameHash);
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

/**
 * @brief 各ジョイントの計算結果から、GPUスキンクラスター用のパレットマトリクスバッファを同期・更新します。
 */
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
