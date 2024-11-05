#pragma once

/// directX12
#include <wrl/client.h>
#include <d3d12.h>

/// std
#include <cstdint>
#include <vector>
#include <array>
#include <span>

/// lib
#include "Math/Matrix4x4.h"

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};


struct JointWeightData {
	Mat4 matBindPoseInverse;
	std::vector<VertexWeightData> vertexWeights;
};


const uint32_t kMaxInfuenceNumber = 4; /// 影響を受けるJointの数
struct VertexInfluence {
	std::array<float,   kMaxInfuenceNumber> weights;
	std::array<int32_t, kMaxInfuenceNumber> jointIndices;
};

struct WellForGPU {
	Mat4 matSkeletonSpace; /// 位置用
	Mat4 matSkeletonSpaceInverseTranspose; /// 法線用
};


struct SkinCluster {

	void FreeDescriptor();

	std::vector<Mat4> matBindPoseInverseArray;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW               vbv;
	std::span<VertexInfluence>             mappedInfluence;

	Microsoft::WRL::ComPtr<ID3D12Resource>                              paletteResource;
	std::span<WellForGPU>                                               mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSRVHandle;
	uint32_t srvDescriptorIndex;

};



SkinCluster CreateSkinCluster(const struct Skeleton& _skeleton, class Model* _model);