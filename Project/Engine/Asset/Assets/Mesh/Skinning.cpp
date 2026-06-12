#include "Skinning.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Tools/StringHash.h"
#include "Model.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animator/Animator.h"

using namespace ONEngine;

Vector3 ANIME_MATH::CalculateValue(const std::vector<KeyFrameVector3>& _keyFrames, float _time) {
	/// ----- Vector3のキーフレーム配列を計算 ----- ///

	Assert(!_keyFrames.empty(), "keyframe empty...");

	/// 最初のキーフレーム以前の場合は最初の値を返す
	if (_keyFrames.size() == 1 || _time <= _keyFrames[0].time) {
		return _keyFrames[0].value;
	}

	/// キーフレーム間の補間計算
	for (size_t index = 0; index < _keyFrames.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (_keyFrames[index].time <= _time && _time <= _keyFrames[nextIndex].time) {
			float t = (_time - _keyFrames[index].time) / (_keyFrames[nextIndex].time - _keyFrames[index].time);
			return Vector3::Lerp(_keyFrames[index].value, _keyFrames[nextIndex].value, t);
		}
	}

	return _keyFrames.back().value;
}

Quaternion ANIME_MATH::CalculateValue(const std::vector<KeyFrameQuaternion>& _keyFrames, float _time) {
	/// ----- Quaternionのキーフレーム配列を計算 ----- ///

	Assert(!_keyFrames.empty(), "keyframe empty...");


	/// 最初のキーフレーム以前の場合は最初の値を返す
	if (_keyFrames.size() == 1 || _time <= _keyFrames[0].time) {
		return _keyFrames[0].value;
	}

	/// キーフレーム間の補間計算
	for (size_t index = 0; index < _keyFrames.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (_keyFrames[index].time <= _time && _time <= _keyFrames[nextIndex].time) {
			float t = (_time - _keyFrames[index].time) / (_keyFrames[nextIndex].time - _keyFrames[index].time);
			return Quaternion::Slerp(_keyFrames[index].value, _keyFrames[nextIndex].value, t);
		}
	}

	return _keyFrames.back().value;
}

int32_t ANIME_MATH::CreateJoint(const Node& _node, const std::optional<int32_t>& _parent, std::vector<Joint>& _joints) {
	/// ----- ノードからジョイントを作成 ----- ///

	/// 自身のインデックスを決定し、追加する
	int32_t selfIndex = static_cast<int32_t>(_joints.size());
	_joints.emplace_back();

	/// 参照を取得 (再確保に注意)
	Joint& joint = _joints[selfIndex];
	joint.name = _node.name;
	joint.nameHash = StringHash::Get(_node.name);
	joint.transform.matWorld = _node.transform.matWorld;

	joint.matSkeletonSpace = Matrix4x4::kIdentity;
	joint.index = selfIndex;
	joint.parent = _parent;

	/// 子ノードのジョイントを再帰的に作成
	for (const Node& child : _node.children) {
		int32_t childIndex = CreateJoint(child, selfIndex, _joints);

		/// 再確保されている可能性があるため、再度インデックスでアクセスして子を追加
		_joints[selfIndex].children.push_back(childIndex);
	}

	return selfIndex;
}

Skeleton ANIME_MATH::CreateSkeleton(const Node& _rootNode) {
	/// ----- ノードからスケルトンを作成 ----- ///
	Console::LogInfo("ANIME_MATH::CreateSkeleton: Starting skeleton creation.");

	Skeleton result;
	result.root = CreateJoint(_rootNode, {}, result.joints);

	for (const Joint& joint : result.joints) {
		result.jointMap.emplace(joint.nameHash, joint.index);
	}

	Console::LogInfo(std::format("ANIME_MATH::CreateSkeleton: Created {} joints.", result.joints.size()));
	return result;
}

SkinCluster ANIME_MATH::CreateSkinCluster(const Skeleton& _skeleton, Asset::Model* _model, DxManager* _dxm) {
	/// ----- スキンクラスターを作成 ----- ///
	Console::LogInfo(std::format("ANIME_MATH::CreateSkinCluster: Starting (Joints: {}, Meshes: {})", 
		_skeleton.joints.size(), _model->GetMeshes().size()));

	SkinCluster result{};

	DxDevice* dxDevice = _dxm->GetDxDevice();
	ID3D12Device* device = dxDevice->GetDevice();

	/// matrix paletteの作成 (ボーンパレットは全メッシュ共通)
	result.paletteResource.CreateResource(dxDevice, sizeof(WellForGPU) * _skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	result.paletteResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	std::memset(mappedPalette, 0, sizeof(WellForGPU) * _skeleton.joints.size());
	result.mappedPalette = { mappedPalette, _skeleton.joints.size() };

	DxSRVHeap* pSRVHeap = _dxm->GetDxSRVHeap();

	/// cpu,gpu handle get
	result.srvDescriptorIndex = pSRVHeap->AllocateBuffer();
	result.paletteSRVHandle.first = pSRVHeap->GetCPUDescriptorHandel(result.srvDescriptorIndex);
	result.paletteSRVHandle.second = pSRVHeap->GetGPUDescriptorHandel(result.srvDescriptorIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSRVDesc{};
	paletteSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSRVDesc.Buffer.FirstElement = 0;
	paletteSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSRVDesc.Buffer.NumElements = static_cast<UINT>(_skeleton.joints.size());
	paletteSRVDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	device->CreateShaderResourceView(result.paletteResource.Get(), &paletteSRVDesc, result.paletteSRVHandle.first);


	/// resource create
	if (_model->GetMeshes().empty()) {
		Console::LogError("ANIME_MATH::CreateSkinCluster: Model has no meshes.");
		return result;
	}

	size_t meshCount = _model->GetMeshes().size();
	result.meshClusters.resize(meshCount);

	/// すべて単位行列で初期化
	result.matBindPoseInverseArray.resize(_skeleton.joints.size());
	std::generate(
		result.matBindPoseInverseArray.begin(), result.matBindPoseInverseArray.end(),
		[]() {return Matrix4x4::kIdentity; }
	);

	// 1. 各メッシュのインフルエンスバッファ作成
	for (size_t meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
		Asset::Model::ModelMesh* mesh = _model->GetMeshes()[meshIndex].get();
		size_t vertexCount = mesh->GetVertices().size();
		auto& meshCluster = result.meshClusters[meshIndex];

		meshCluster.influenceResource.CreateResource(dxDevice, sizeof(VertexInfluence) * vertexCount);

		/// mapping
		VertexInfluence* mappedInfluence = nullptr;
		meshCluster.influenceResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * vertexCount);
		meshCluster.mappedInfluence = { mappedInfluence, vertexCount };

		/// vbvの作成
		meshCluster.vbv.BufferLocation = meshCluster.influenceResource.Get()->GetGPUVirtualAddress();
		meshCluster.vbv.SizeInBytes = static_cast<UINT>(sizeof(VertexInfluence) * vertexCount);
		meshCluster.vbv.StrideInBytes = sizeof(VertexInfluence);

		// 2. そのメッシュのウェイトデータを流し込む
		const auto& jointWeights = _model->GetMeshJointWeightData()[meshIndex];

		for (const auto& [jointHash, weightData] : jointWeights) {
			auto itr = _skeleton.jointMap.find(jointHash);
			if (itr == _skeleton.jointMap.end()) continue;

			uint32_t skeletonJointIndex = itr->second;
			result.matBindPoseInverseArray[skeletonJointIndex] = weightData.matBindPoseInverse;

			for (const auto& vertexWeight : weightData.vertexWeights) {
				if (vertexWeight.vertexIndex >= vertexCount) {
					// Console::LogWarning(std::format("ANIME_MATH::CreateSkinCluster: Vertex index {} is out of bounds for mesh {} (size: {}).", vertexWeight.vertexIndex, meshIndex, vertexCount));
					continue;
				}

				VertexInfluence& influence = meshCluster.mappedInfluence[vertexWeight.vertexIndex];
				for (uint32_t i = 0u; i < kMaxInfluenceNumber; ++i) {
					if (influence.weights[i] == 0.0f) {
						influence.weights[i] = vertexWeight.weight;
						influence.jointIndices[i] = static_cast<int32_t>(skeletonJointIndex);
						break;
					}
				}
			}
		}
	}

	Console::LogInfo("ANIME_MATH::CreateSkinCluster: Finished.");

	return result;
}

void ANIME_MATH::SampleAnimation(const std::unordered_map<uint32_t, AnimationClip>& _clips, const AnimationState& _state, uint32_t _jointNameHash, SampledTransform& _outTransform) {
    auto itClip = _clips.find(_state.clipId);
    if (itClip == _clips.end()) return;

    const AnimationClip& clip = itClip->second;

    auto itAnim = clip.nodeAnimationMap.find(_jointNameHash);
    if (itAnim == clip.nodeAnimationMap.end()) return;

    const NodeAnimation& anim = itAnim->second;

    if (!anim.translate.empty()) _outTransform.translate = CalculateValue(anim.translate, _state.time);
    if (!anim.rotate.empty()) _outTransform.rotate = CalculateValue(anim.rotate, _state.time);
    if (!anim.scale.empty()) _outTransform.scale = CalculateValue(anim.scale, _state.time);
}
