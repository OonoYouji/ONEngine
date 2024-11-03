#include "AnimationRenderer.h"

/// std
#include <format>

/// external
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

/// engine
#include <imgui.h>
#include "Core/ONEngine.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/ModelManager/Model.h"
#include "GameObjectManager/BaseGameObject.h"

/// grahics
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"

/// objects
#include "Objects/Camera/Manager/CameraManager.h"
#include "GameObjectManager/BaseGameObject.h"

/// lib
#include "Debugger/Assertion.h"


AnimationRenderer::AnimationRenderer(const std::string& modelFilePath) {
	pModel_ = ModelManager::Load(modelFilePath);
	LoadAnimation(modelFilePath);
}
AnimationRenderer::~AnimationRenderer() {
	skinCluster_.FreeDescriptor();
}


void AnimationRenderer::Initialize() {
	transform_.rotateOrder = QUATERNION;

	skeleton_    = CreateSkeleton(pModel_->GetRootNode());
	skinCluster_ = CreateSkinCluster(skeleton_, pModel_);
}

void AnimationRenderer::Update() {
	animationTime_ += Time::DeltaTime();
	animationTime_ = std::fmod(animationTime_, duration_);

	NodeAnimation& rootAnimation = nodeAnimationArray_[pModel_->GetRootNode().name];

	//transform_.position   = CalculateValue(rootAnimation.translate, animationTime_);
	//transform_.quaternion = CalculateValue(rootAnimation.rotate,    animationTime_);
	//transform_.scale      = CalculateValue(rootAnimation.scale,     animationTime_);
	//transform_.Update();

	skeleton_.Update(duration_, nodeAnimationArray_);
	SkinClusterUpdate(skinCluster_, skeleton_);

}

void AnimationRenderer::Draw() {
	AnimationRendererCommon* instance =	AnimationRendererCommon::GetInstance();
	instance->AddAnimationRenderer(this);
}

void AnimationRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(std::format("position : {:.2f}, {:.2f}, {:.2f}",         transform_.position.x,   transform_.position.y,   transform_.position.z).c_str());
		ImGui::Text(std::format("rotate   : {:.2f}, {:.2f}, {:.2f}, {:.2f}", transform_.quaternion.x, transform_.quaternion.y, transform_.quaternion.z, transform_.quaternion.w).c_str());
		ImGui::Text(std::format("scale    : {:.2f}, {:.2f}, {:.2f}",         transform_.scale.x,      transform_.scale.y,      transform_.scale.z).c_str());


		ImGui::SeparatorText("local matrix");

		for(uint32_t r = 0u; r < 4; ++r) {
			for(uint32_t c = 0u; c < 4; ++c) {
				if(c != 0) {
					ImGui::SameLine();
				}

				ImGui::Text("%0.2f, ", transform_.matTransform.m[r][c]);
			}
		}


		ImGui::SeparatorText("parameters");

		ImGui::Text(std::format("animation time : {:.2f}", animationTime_).c_str());
		ImGui::Text(std::format("duration       : {:.2f}", duration_).c_str());

		ImGui::TreePop();
	}
}

void AnimationRenderer::DrawCall() {
	ID3D12GraphicsCommandList* pCommandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	ID3D12Resource*            pViewBuffer  = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();
	std::vector<Mesh>&         meshArray    = pModel_->GetMeshes();

	/// default setting
	pCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// buffer setting
	pCommandList->SetGraphicsRootConstantBufferView(0, pViewBuffer->GetGPUVirtualAddress());
	pCommandList->SetGraphicsRootDescriptorTable(2, skinCluster_.paletteSRVHandle.second);

	for(auto& mesh : meshArray) {

		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			mesh.GetVBV(), skinCluster_.vbv
		};

		pCommandList->IASetVertexBuffers(0, 2, vbvs);
		pCommandList->IASetIndexBuffer(&mesh.GetIBV());
		GetOwner()->GetTransform()->BindTransform(pCommandList, 1);

		pCommandList->DrawIndexedInstanced(
			static_cast<UINT>(mesh.GetIndices().size()),
			1, 0, 0, 0
		);
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

void AnimationRenderer::ApplyAnimation(Skeleton& skeleton) {
	for(Joint& joint : skeleton.joints) {

		auto itr = nodeAnimationArray_.find(joint.name);
		if(itr != nodeAnimationArray_.end()) {
			const NodeAnimation& rootNodeAnimation = (*itr).second;

			joint.transform.position   = CalculateValue(rootNodeAnimation.translate, animationTime_);
			joint.transform.quaternion = CalculateValue(rootNodeAnimation.rotate,    animationTime_);
			joint.transform.scale      = CalculateValue(rootNodeAnimation.scale,     animationTime_);
		}
	}
}

void AnimationRenderer::SkinClusterUpdate(SkinCluster& _skinCluster, const Skeleton& _skeleton) const {
	for(size_t jointIndex = 0; jointIndex < _skeleton.joints.size(); ++jointIndex) {

		Assert(jointIndex < _skinCluster.matBindPoseInverseArray.size(), "out of range");

		_skinCluster.mappedPalette[jointIndex].matSkeletonSpace =
			_skinCluster.matBindPoseInverseArray[jointIndex] * _skeleton.joints[jointIndex].matSkeletonSpace;

		_skinCluster.mappedPalette[jointIndex].matSkeletonSpaceInverseTranspose =
			Mat4::MakeTranspose(Mat4::MakeInverse(_skinCluster.mappedPalette[jointIndex].matSkeletonSpace));

	}
}




void AnimationRendererCommon::Initialize() {

	shader_.ShaderCompile(
		L"Skinning/Skinning.VS.hlsl", L"vs_6_0",
		L"Skinning/Skinning.PS.hlsl", L"ps_6_0"
	);


	pipelineState_.reset(new PipelineState);

	pipelineState_->SetShader(&shader_);
	pipelineState_->SetFillMode(kSolid);
	pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	///	struct VSInput {
	///		float4 position : POSITON0;
	///		float2 texcoord : TEXCOORD0;
	///		float3 normal : NORMAL0;
	///		float4 weight : WEIGHT0;
	///		int4   index : INDEX0;
	///	};

	/// ---------------------------------------------------
	/// input layout setting
	/// ---------------------------------------------------

	pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineState_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pipelineState_->AddInputElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

	std::array<D3D12_INPUT_ELEMENT_DESC, 2> inputElementDescs{};
	inputElementDescs[0].SemanticName      = "WEIGHT";
	inputElementDescs[0].SemanticIndex     = 0;
	inputElementDescs[0].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].InputSlot         = 1;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName      = "INDEX";
	inputElementDescs[1].SemanticIndex     = 0;
	inputElementDescs[1].Format            = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs[1].InputSlot         = 1;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	pipelineState_->AddInputElement(inputElementDescs[0]);
	pipelineState_->AddInputElement(inputElementDescs[1]);


	/// ---------------------------------------------------
	/// buffer setting
	/// ---------------------------------------------------

	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// gViewProjection
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// gTransform

	/// gMatrixPalette
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	pipelineState_->Initialize();
}

void AnimationRendererCommon::Finalize() {
	pipelineState_.reset();
}

void AnimationRendererCommon::PreDraw() {
	actives_.clear();
}

void AnimationRendererCommon::PostDraw() {
	pipelineState_->SetPipelineState();
	for(auto& animationRenderer : actives_) {
		animationRenderer->DrawCall();
	}
}

void AnimationRendererCommon::AddAnimationRenderer(AnimationRenderer* _animationRenderer) {
	actives_.push_back(_animationRenderer);
}
