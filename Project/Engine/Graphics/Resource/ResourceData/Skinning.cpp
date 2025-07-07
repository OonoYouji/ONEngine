#include "Skinning.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Resource/ResourceData/Model.h"

Vector3 ANIME_MATH::CalculateValue(const std::vector<KeyFrameVector3>& _keyFrames, float _time) {
	Assert(!_keyFrames.empty(), "keyframe empty...");

	if (_keyFrames.size() == 1 || _time <= _keyFrames[0].time) {
		return _keyFrames[0].value;
	}

	for (size_t index = 0; index < _keyFrames.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (_keyFrames[index].time <= _time && _time <= _keyFrames[nextIndex].time) {
			float t = (_time - _keyFrames[index].time) / (_keyFrames[nextIndex].time - _keyFrames[index].time);
			return Vec3::Lerp(_keyFrames[index].value, _keyFrames[nextIndex].value, t);
		}
	}

	return _keyFrames.back().value;
}

Quaternion ANIME_MATH::CalculateValue(const std::vector<KeyFrameQuaternion>& _keyFrames, float _time) {
	Assert(!_keyFrames.empty(), "keyframe empty...");

	if (_keyFrames.size() == 1 || _time <= _keyFrames[0].time) {
		return _keyFrames[0].value;
	}

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
	Joint joint;

	joint.name = _node.name;
	joint.transform.matWorld = _node.transform.matWorld;
	joint.matSkeletonSpace = Matrix4x4::kIdentity;
	joint.index = static_cast<int32_t>(_joints.size());
	joint.parent = _parent;
	_joints.push_back(joint);

	for (const Node& child : _node.children) {
		int32_t childIndex = CreateJoint(child, joint.index, _joints);
		joint.children.push_back(childIndex);
	}

	return joint.index;
}

Skeleton ANIME_MATH::CreateSkeleton(const Node& _rootNode) {
	Skeleton result;
	result.root = CreateJoint(_rootNode, {}, result.joints);

	for (const Joint& joint : result.joints) {
		result.jointMap.emplace(joint.name, joint.index);
	}

	return result;
}

SkinCluster ANIME_MATH::CreateSkinCluster(const Skeleton& _skeleton, Model* _model, DxManager* _dxManager) {
	SkinCluster result{};

	DxDevice* dxDevice = _dxManager->GetDxDevice();
	/// matrix paletteの作成
	result.paletteResource.CreateResource(dxDevice, sizeof(WellForGPU) * _skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	result.paletteResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	result.mappedPalette = { mappedPalette, _skeleton.joints.size() };

	DxSRVHeap* pSRVHeap = _dxManager->GetDxSRVHeap();

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

	ID3D12Device* pDevice = _dxManager->GetDxDevice()->GetDevice();
	pDevice->CreateShaderResourceView(result.paletteResource.Get(), &paletteSRVDesc, result.paletteSRVHandle.first);



	/// influence resource create

	/*##########################################################
		TODO : COMMENT
		とりあえずmulti meshを考えないで先頭のmeshのみを使用するようにしたので
		multi mesh対応は後ほど考える
	##########################################################*/


	/// resource create
	Mesh* frontMesh = _model->GetMeshes().front().get();
	result.influenceResource.CreateResource(dxDevice, sizeof(VertexInfluence) * frontMesh->GetVertices().size());

	/// mapping
	VertexInfluence* mappedInfluence = nullptr;
	result.influenceResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * frontMesh->GetVertices().size()); /// 第二引数の0ですべて初期化する
	result.mappedInfluence = { mappedInfluence, frontMesh->GetVertices().size() };

	/// vbvの作成
	result.vbv.BufferLocation = result.influenceResource.Get()->GetGPUVirtualAddress();
	result.vbv.SizeInBytes = static_cast<UINT>(sizeof(VertexInfluence) * frontMesh->GetVertices().size());
	result.vbv.StrideInBytes = sizeof(VertexInfluence);

	result.matBindPoseInverseArray.resize(_skeleton.joints.size());
	std::generate(
		result.matBindPoseInverseArray.begin(), result.matBindPoseInverseArray.end(),
		[]() {return Matrix4x4::kIdentity; }
	); /// すべて単位行列で初期化


	for (const auto& jointWeight : _model->GetJointWeightData()) {

		/// jointWeight.firstにあるjoint名からskeletonに対象のjointがあるか探索
		/// なければ次の処理
		auto itr = _skeleton.jointMap.find(jointWeight.first);
		if (itr == _skeleton.jointMap.end()) {
			continue;
		}

		result.matBindPoseInverseArray[(*itr).second] = jointWeight.second.matBindPoseInverse;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {

			/// influenceの開いている箇所に入れる
			VertexInfluence& currentInfluence = result.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0u; index < kMaxInfluenceNumber; ++index) {

				/// 空いている
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*itr).second;
					break;
				}
			}

		}

	}

	return result;
}
