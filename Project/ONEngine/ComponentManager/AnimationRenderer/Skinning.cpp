#include "Skinning.h"

/// std
#include <algorithm>

#include "Core/ONEngine.h"

/// graphics
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"

#include "GraphicManager/ModelManager/Model.h"
#include "Skeleton.h"

using namespace ONE;


SkinCluster CreateSkinCluster(const Skeleton& _skeleton, Model* _model) {
	SkinCluster result{};

	/// matrix paletteの作成
	result.paletteResource = ONE::DxResourceCreator::CreateResource(sizeof(WellForGPU) * _skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	result.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	result.mappedPalette = { mappedPalette, _skeleton.joints.size() };

	DxDescriptorHeap<HeapType::CBV_SRV_UAV>* pSRVHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();

	/// cpu,gpu handle get
	result.srvDescriptorIndex = pSRVHeap->Allocate();
	result.paletteSRVHandle.first  = pSRVHeap->GetCPUDescriptorHandel(result.srvDescriptorIndex);
	result.paletteSRVHandle.second = pSRVHeap->GetGPUDescriptorHandel(result.srvDescriptorIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSRVDesc{};
	paletteSRVDesc.Format                     = DXGI_FORMAT_UNKNOWN;
	paletteSRVDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSRVDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
	paletteSRVDesc.Buffer.FirstElement        = 0;
	paletteSRVDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSRVDesc.Buffer.NumElements         = static_cast<UINT>(_skeleton.joints.size());
	paletteSRVDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	ID3D12Device* pDevice = ONEngine::GetDxCommon()->GetDevice();
	pDevice->CreateShaderResourceView(result.paletteResource.Get(), &paletteSRVDesc, result.paletteSRVHandle.first);



	/// influence resource create

	/*##########################################################
		TODO : COMMENT
		とりあえずmulti meshを考えないで先頭のmeshのみを使用するようにしたので
		multi mesh対応は後ほど考える
	##########################################################*/


	/// resource create
	Mesh& frontMesh = _model->GetMeshes().front();
	result.influenceResource = DxResourceCreator::CreateResource(sizeof(VertexInfluence) * frontMesh.GetVertices().size());

	/// mapping
	VertexInfluence* mappedInfluence = nullptr;
	result.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * frontMesh.GetVertices().size()); /// 第二引数の0ですべて初期化する
	result.mappedInfluence = { mappedInfluence, frontMesh.GetVertices().size() };

	/// vbvの作成
	result.vbv.BufferLocation = result.influenceResource->GetGPUVirtualAddress();
	result.vbv.SizeInBytes = static_cast<UINT>(sizeof(VertexInfluence) * frontMesh.GetVertices().size());
	result.vbv.StrideInBytes = sizeof(VertexInfluence);

	result.matBindPoseInverseArray.resize(_skeleton.joints.size());
	std::generate(
		result.matBindPoseInverseArray.begin(), result.matBindPoseInverseArray.end(),
		[]() {return Mat4::kIdentity; }
	); /// すべて単位行列で初期化


	for(const auto& jointWeight : _model->GetSkinClusterData()) {

		/// jointWeight.firstにあるjoint名からskeletonに対象のjointがあるか探索
		/// なければ次の処理
		auto itr = _skeleton.jointMap.find(jointWeight.first);
		if(itr == _skeleton.jointMap.end()) {
			continue;
		}

		result.matBindPoseInverseArray[(*itr).second] = jointWeight.second.matBindPoseInverse;
		for(const auto& vertexWeight : jointWeight.second.vertexWeights) {

			/// influenceの開いている箇所に入れる
			VertexInfluence& currentInfluence = result.mappedInfluence[vertexWeight.vertexIndex];
			for(uint32_t index = 0u; index < kMaxInfuenceNumber; ++index) {

				/// 空いている
				if(currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index]      = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*itr).second;
					break;
				}
			}

		}

	}

	return result;
}

void SkinCluster::FreeDescriptor() {
	DxDescriptorHeap<HeapType::CBV_SRV_UAV>* pSRVHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
	pSRVHeap->Free(srvDescriptorIndex);
}
