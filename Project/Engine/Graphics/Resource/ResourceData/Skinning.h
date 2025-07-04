#pragma once

/// directX
#include <d3d12.h>

/// std
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <array>
#include <span>
#include <unordered_map>

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Engine/Core/Utility/Math/Quaternion.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"

struct Joint {
	Transform transform;
	Matrix4x4 matSkeletonSpace;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

struct Node {
	Transform transform;
	std::string name;
	std::vector<Node> children;
};

struct VertexWeightDara {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 matBindPoseInverse;
	std::vector<VertexWeightDara> vertexWeights;
};

const uint32_t kMaxInfluenceNumber = 4; ///< 最大の影響を受けるジョイント数
struct VertexInfluence {
	std::array<float, kMaxInfluenceNumber> weights;
	std::array<int32_t, kMaxInfluenceNumber> jointIndices;
};

struct WellForGPU {
	Matrix4x4 matSkeletonSpace;
	Matrix4x4 matSkeletonSpaceInverseTranspose;
};

struct SkinCluster {
	std::vector<Matrix4x4> matBindPoseInverseArray;
	DxResource influenceResource;
	D3D12_VERTEX_BUFFER_VIEW vbv;
	std::span<VertexInfluence> mappedInfluence; 
	DxResource paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSRVHandle;
	uint32_t srvDescriptorIndex;
};

template<typename T>
struct KeyFrame {
	float time;
	T value;
};

using KeyFrameVector3 = KeyFrame<Vector3>;
using KeyFrameQuaternion = KeyFrame<Quaternion>;

struct NodeAnimation {
	std::vector<KeyFrameVector3> translate;
	std::vector<KeyFrameQuaternion> rotate;
	std::vector<KeyFrameVector3> scale;
};

struct Skeleton {
	int32_t root;
	std::unordered_map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};


/// 前方宣言
class Model;
class DxManager;

namespace MATH {
	Vector3 CalculateValue(const std::vector<KeyFrameVector3>& _keyFrames, float _time);
	Quaternion CalculateValue(const std::vector<KeyFrameQuaternion>& _keyFrames, float _time);

	int32_t CreateJoint(const Node& _node, const std::optional<int32_t>& _parent, std::vector<Joint>& _joints);
	Skeleton CreateSkeleton(const Node& _rootNode);
	SkinCluster CreateSkinCluster(const Skeleton& _skeleton, Model* _model, DxManager* _dxManager);
}

