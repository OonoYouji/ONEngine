#include "AnimationRenderer.h"

/// std
#include <format>

/// external
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/ModelManager/Model.h"
#include "GameObjectManager/BaseGameObject.h"

/// lib
#include "Debug/Assert.h"


AnimationRenderer::AnimationRenderer() {}
AnimationRenderer::~AnimationRenderer() {}


void AnimationRenderer::Initialize() {

}

void AnimationRenderer::Update() {
	animationTime_ += Time::DeltaTime();
	animationTime_ = std::fmod(animationTime_, duration_);

	NodeAnimation& rootAnimation = nodeAnimationArray_[pModel_->GetRootNode().name];

	position_ = CalculateValue(rootAnimation.translate, animationTime_);
	rotate_   = CalculateValue(rootAnimation.rotate,    animationTime_);
	scale_    = CalculateValue(rootAnimation.scale,     animationTime_);

	matLocal_ = Mat4::MakeScale(scale_) * Mat4::MakeRotateQuaternion(Quaternion::Normalize(rotate_)) * Mat4::MakeTranslate(position_);

}

void AnimationRenderer::Draw() {
	pModel_->Draw(GetOwner()->GetTransform(), &matLocal_, nullptr, kSolid);
}

void AnimationRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(std::format("position : {:.2f}, {:.2f}, {:.2f}",         position_.x, position_.y, position_.z).c_str());
		ImGui::Text(std::format("rotate   : {:.2f}, {:.2f}, {:.2f}, {:.2f}", rotate_.x, rotate_.y, rotate_.z, rotate_.w).c_str());
		ImGui::Text(std::format("scale    : {:.2f}, {:.2f}, {:.2f}",         scale_.x, scale_.y, scale_.z).c_str());


		ImGui::SeparatorText("local matrix");

		for(uint32_t r = 0u; r < 4; ++r) {
			for(uint32_t c = 0u; c < 4; ++c) {
				if(c != 0) {
					ImGui::SameLine();
				}

				ImGui::Text("%0.2f, ", matLocal_.m[r][c]);
			}
		}


		ImGui::SeparatorText("parameters");

		ImGui::Text(std::format("animation time : {:.2f}", animationTime_).c_str());
		ImGui::Text(std::format("duration       : {:.2f}", duration_).c_str());

		ImGui::TreePop();
	}
}




void AnimationRenderer::SetModel(const std::string& filePath) {
	pModel_ = ModelManager::Load(filePath);
}

void AnimationRenderer::LoadAnimation(const std::string& filePath) {

	ModelManager* pModelManager = ModelManager::GetInstance();

	Assimp::Importer importer;
	std::string    objPath = pModelManager->GetDirectoryPath() + filePath + "/" + filePath + ".gltf";
	const aiScene* scene   = importer.ReadFile(objPath.c_str(), 0);

	Assert(scene->mAnimations != 0, "no animation...");

	/// 解析
	aiAnimation* animationAssimp = scene->mAnimations[0];

	duration_ = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	/// node animationの読み込み
	for(uint32_t channelIndex = 0u; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

		/// node animationの解析用データを
		aiNodeAnim*    nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation       = nodeAnimationArray_[nodeAnimationAssimp->mNodeName.C_Str()];
		
		/// ---------------------------------------------------
		/// translateの解析
		/// ---------------------------------------------------
		for(uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {

			/// keyの値を得る
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVec3 keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }
			};

			nodeAnimation.translate.push_back(keyframe);
		}

		
		/// ---------------------------------------------------
		/// rotateの解析
		/// ---------------------------------------------------
		for(uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {

			/// keyの値を得る
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }
			};

			nodeAnimation.rotate.push_back(keyframe);
		}

		
		/// ---------------------------------------------------
		/// scaleの解析
		/// ---------------------------------------------------
		for(uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {

			/// keyの値を得る
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVec3 keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }
			};

			nodeAnimation.scale.push_back(keyframe);
		}

	}

}



Vec3 AnimationRenderer::CalculateValue(const std::vector<KeyframeVec3>& keyframeArray, float time) {
	Assert(!keyframeArray.empty(), "keyframe empty...");

	if(keyframeArray.size() == 1 || time <= keyframeArray[0].time) {
		return keyframeArray[0].value;
	}

	for(size_t index = 0; index < keyframeArray.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if(keyframeArray[index].time <= time && time <= keyframeArray[nextIndex].time) {
			float t = (time - keyframeArray[index].time) / (keyframeArray[nextIndex].time - keyframeArray[index].time);
			return Vec3::Lerp(keyframeArray[index].value, keyframeArray[nextIndex].value, t);
		}
	}

	return keyframeArray.back().value;
}

Quaternion AnimationRenderer::CalculateValue(const std::vector<KeyframeQuaternion>& keyframeArray, float time) {
	Assert(!keyframeArray.empty(), "keyframe empty...");

	if(keyframeArray.size() == 1 || time <= keyframeArray[0].time) {
		return keyframeArray[0].value;
	}

	for(size_t index = 0; index < keyframeArray.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if(keyframeArray[index].time <= time && time <= keyframeArray[nextIndex].time) {
			float t = (time - keyframeArray[index].time) / (keyframeArray[nextIndex].time - keyframeArray[index].time);
			return Quaternion::Lerp(keyframeArray[index].value, keyframeArray[nextIndex].value, t);
		}
	}

	return keyframeArray.back().value;
}