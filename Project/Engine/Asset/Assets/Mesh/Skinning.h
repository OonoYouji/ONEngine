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


namespace ONEngine {
class DxManager;
}

namespace ONEngine::Asset {
class Model;
}


namespace ONEngine {
struct AnimationState;
}

namespace ONEngine {

/// @brief アニメーションのジョイント情報
struct Joint {
	Transform transform;
	Matrix4x4 matSkeletonSpace;
	Matrix4x4 matWorld;
	std::string name;
	uint32_t nameHash = 0;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

/// @brief ノード情報
struct Node {
	Transform transform;
	std::string name;
	std::vector<Node> children;
};

/// @brief 頂点のウェイト情報
struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

/// @brief ジョイントのウェイトデータ
struct JointWeightData {
	Matrix4x4 matBindPoseInverse;
	std::vector<VertexWeightData> vertexWeights;
};

/// @brief 頂点の影響情報
const uint32_t kMaxInfluenceNumber = 4; ///< 最大の影響を受けるジョイント数
struct VertexInfluence {
	std::array<float, kMaxInfluenceNumber> weights;
	std::array<int32_t, kMaxInfluenceNumber> jointIndices;
};

/// @brief GPU用のウェル情報
struct WellForGPU {
	Matrix4x4 matSkeletonSpace;
	Matrix4x4 matSkeletonSpaceInverseTranspose;
};


/// @brief メッシュごとのスキンクラスター情報
struct MeshSkinCluster {
	DxResource influenceResource;
	D3D12_VERTEX_BUFFER_VIEW vbv;
	std::span<VertexInfluence> mappedInfluence;
};

/// @brief スキンクラスター情報
struct SkinCluster {
	std::vector<Matrix4x4> matBindPoseInverseArray;
	std::vector<MeshSkinCluster> meshClusters;
	DxResource paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSRVHandle;
	uint32_t srvDescriptorIndex;
};

/// @brief キーフレーム構造体
/// @tparam T Vector3 or Quaternion
template<typename T>
struct KeyFrame {
	float time;
	T value;
};

/// @brief using宣言
using KeyFrameVector3 = KeyFrame<Vector3>;
using KeyFrameQuaternion = KeyFrame<Quaternion>;

/// @brief ノードのアニメーション情報 SRT
struct NodeAnimation {
	std::vector<KeyFrameVector3> translate;
	std::vector<KeyFrameQuaternion> rotate;
	std::vector<KeyFrameVector3> scale;
};

/// @brief アニメーションイベント
struct AnimationEvent {
	std::string eventName;
	uint32_t eventNameHash;
	float time; // 0.0 ~ 1.0 (normalized)
};

/// @brief アニメーションクリップ
struct AnimationClip {
	std::string name;
	uint32_t nameHash;
	std::unordered_map<uint32_t, NodeAnimation> nodeAnimationMap;
	std::vector<AnimationEvent> events;
	float duration;
};

/// @brief スケルトン情報
struct Skeleton {
	int32_t root;
	std::unordered_map<uint32_t, int32_t> jointMap;
	std::vector<Joint> joints;
};

/// @brief ボーンマスク情報
struct BoneMask {
	std::string name;
	uint32_t nameHash = 0;
	std::unordered_map<uint32_t, float> jointWeights; // jointNameHash -> weight (0.0 ~ 1.0)
};


namespace ANIME_MATH {

	/// @brief Vector3のキーフームを基に補間計算を行う
	/// @param _keyFrames Vector3のキーフレーム配列
	/// @param _time 補間時間
	/// @return 補間後のVector3値
	Vector3 CalculateValue(const std::vector<KeyFrameVector3>& _keyFrames, float _time);

	/// @brief Quaternionのキーフームを基に補間計算を行う
	/// @param _keyFrames Quaternionのキーフーム配列
	/// @param _time 補間時間
	/// @return 補間後のQuaternion値
	Quaternion CalculateValue(const std::vector<KeyFrameQuaternion>& _keyFrames, float _time);


	/// @brief ノードからジョイントを作成
	/// @param _node ソースのノード
	/// @param _parent 親子関係を示す親のインデックス
	/// @param _joints Joint配列への参照
	/// @return 生成されたJointのインデックス
	int32_t CreateJoint(const Node& _node, const std::optional<int32_t>& _parent, std::vector<Joint>& _joints);

	/// @brief モデルのスケルトン構築
	/// @param _rootNode ソースのルートノード
	/// @return 構築されたスケルトン
	Skeleton CreateSkeleton(const Node& _rootNode);

	/// @brief スキンクラスターの作成
	/// @param _skeleton CreateSkeletonで作成されたスケルトン
	/// @param _model ソースモデル
	/// @param _dxm DxManagerのインスタンスへのポインタ
	/// @return 構築されたスキンクラスター
	SkinCluster CreateSkinCluster(const Skeleton& _skeleton, Asset::Model* _model, DxManager* _dxm);

	/// @brief アニメーションのサンプリング結果
	struct SampledTransform {
		Vector3 translate = { 0,0,0 };
		Quaternion rotate = { 0,0,0,1 };
		Vector3 scale = { 1,1,1 };
	};

	/// @brief 複数のアニメーションをブレンドしてサンプリング
	/// @param _clips モデルに含まれる全クリップ
	/// @param _state 再生状態
	/// @param _jointNameHash ジョイント名のハッシュ
	/// @param _outTransform 結果格納先
	void SampleAnimation(const std::unordered_map<uint32_t, AnimationClip>& _clips, const AnimationState& _state, uint32_t _jointNameHash, SampledTransform& _outTransform);
}


} /// ONEngine
