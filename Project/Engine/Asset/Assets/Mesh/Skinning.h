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

	/**
	 * @brief キーフレーム情報（Vector3）に基づき、指定時間の線形補間値を計算します。
	 * @param _keyFrames キーフレームリスト
	 * @param _time 取得したい時点の時間
	 * @return 補間されたVector3オブジェクト
	 */
	Vector3 CalculateValue(const std::vector<KeyFrameVector3>& _keyFrames, float _time);

	/**
	 * @brief キーフレーム情報（Quaternion）に基づき、指定時間の球面線形補間値を計算します。
	 * @param _keyFrames キーフレームリスト
	 * @param _time 取得したい時点の時間
	 * @return 補間されたQuaternionオブジェクト
	 */
	Quaternion CalculateValue(const std::vector<KeyFrameQuaternion>& _keyFrames, float _time);


	/**
	 * @brief アセットのノード木構造から、再帰的にジョイントを構築し、リストに追加します。
	 * @param _node 起点となるノード
	 * @param _parent 親ジョイントのインデックス（存在しない場合はnullopt）
	 * @param _joints 追加先のジョイント配列
	 * @return 生成されたジョイントのインデックス
	 */
	int32_t CreateJoint(const Node& _node, const std::optional<int32_t>& _parent, std::vector<Joint>& _joints);

	/**
	 * @brief ルートノードからスケルトン構造（ジョイントの集合・親子関係マップ）を構築します。
	 * @param _rootNode ルートノード
	 * @return 構築されたSkeletonオブジェクト
	 */
	Skeleton CreateSkeleton(const Node& _rootNode);

	/**
	 * @brief GPUでスキンメッシュアニメーションを行うためのスキンクラスター（バッファ、パレット等）を作成します。
	 * @param _skeleton ジョイント構造を持つスケルトン
	 * @param _model 対象のModelアセット
	 * @param _dxm DirectX12マネージャのポインタ
	 * @return 作成されたSkinClusterオブジェクト
	 */
	SkinCluster CreateSkinCluster(const Skeleton& _skeleton, Asset::Model* _model, DxManager* _dxm);

	/**
	 * @brief アニメーションサンプリング結果構造体
	 */
	struct SampledTransform {
		Vector3 translate = { 0,0,0 };    ///< 平行移動値
		Quaternion rotate = { 0,0,0,1 }; ///< 回転クォータニオン
		Vector3 scale = { 1,1,1 };       ///< スケール値
	};

	/**
	 * @brief 現在のアニメーション再生状態およびブレンド割合に基づいて、特定ジョイントの補間SRTデータをサンプリングします。
	 * @param _clips モデルに含まれる全アニメーションクリップマップ
	 * @param _state 現在のアニメーション再生状態（ブレンド元情報など）
	 * @param _jointNameHash 対象ジョイント名のハッシュ
	 * @param _outTransform サンプリング結果を格納する構造体の参照
	 */
	void SampleAnimation(const std::unordered_map<uint32_t, AnimationClip>& _clips, const AnimationState& _state, uint32_t _jointNameHash, SampledTransform& _outTransform);
}


} /// ONEngine
