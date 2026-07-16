#pragma once

/// std
#include <vector>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Terrain.h"


/// @brief  Debug関数様に前方宣言
namespace ONEngine {

class TerrainCollider;

/// @brief Componentのデバッグ表示
namespace ComponentDebug {
    /**
     * @brief エディタ用：TerrainColliderコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
    void TerrainColliderDebug(TerrainCollider* _collider);
}

/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, TerrainCollider& _c);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const TerrainCollider& _c);

/**
 * @class TerrainCollider
 * @brief 地形（Terrain）メッシュの凹凸形状に基づいてリアルタイムの高さクエリや最大斜度チェック、勾配（法線）取得などを行う衝突判定用コライダーコンポーネントクラス
 */
class TerrainCollider : public IComponent {
	/// ----- friend class  ----- ///
	friend class TerrainColliderVertexGenerator;

	/// ----- friend functions ----- ///
	friend void ComponentDebug::TerrainColliderDebug(TerrainCollider* _collider);
	friend void from_json(const nlohmann::json& _j, TerrainCollider& _c);
	friend void to_json(nlohmann::json& _j, const TerrainCollider& _c);

public:
	/// =========================================
	/// public : methods
	/// =========================================

	/**
	 * @brief コンストラクタ
	 */
	TerrainCollider();

	/**
	 * @brief デストラクタ
	 */
	~TerrainCollider() override = default;

	/**
	 * @brief 同一Entity内にある地形（Terrain）コンポーネントをアタッチ・関連付けます。
	 */
	void AttachTerrain();

	/**
	 * @brief GPU側で変形された地形の頂点バッファデータをCPU側の二次元配列（グリッド情報）へ読み込みコピーします。
	 */
	void CopyVertices(class DxManager* _dxm);

	/**
	 * @brief 指定されたワールド座標点（XZ）における地形の正確な高さを取得（グリッド間はバイリニア補間）します。
	 * @param _position 高さを調べたいワールド座標
	 * @return 補間された高さ（Y座標）
	 */
	float GetHeight(const Vector3& _position);

	/**
	 * @brief 指定されたワールド座標点（XZ）における地形斜面の勾配（傾斜方向ベクトル）を算出します。
	 * @param _position 勾配を調べたいワールド座標
	 * @return 勾配方向ベクトル
	 */
	Vector3 GetGradient(const Vector3& _position);

	/**
	 * @brief 指定されたワールド座標が地形のXZ範囲内に含まれているかを判定します。
	 * @param _position 判定したいワールド座標
	 * @return 範囲内にある場合は true、範囲外の場合は false
	 */
	bool IsInsideTerrain(const Vector3& _position);


private:
	/// =========================================
	/// private : objects
	/// =========================================

	/// ----- other class  ----- ///
	Terrain* pTerrain_;

	std::vector<std::vector<TerrainVertex>> vertices_;

	bool isVertexGenerationRequested_;
	bool isCreated_;

	float maxSlopeAngle_;

public:
	/// =========================================
	/// public : accessor
	/// =========================================

	/**
	 * @brief 関連付けられている地形コンポーネントへのポインタを取得します。
	 */
	Terrain* GetTerrain() const;

	/**
	 * @brief コライダーが保持する地形頂点グリッドの二次元配列を読み取り専用で取得します。
	 */
	const std::vector<std::vector<TerrainVertex>>& GetVertices() const;

	/**
	 * @brief コライダーが保持する地形頂点グリッドの二次元配列を取得します。
	 */
	std::vector<std::vector<TerrainVertex>>& GetVertices();

	/**
	 * @brief コライダーデータの構築が完了しているかを取得します。
	 */
	bool GetIsCreated() const;


	/**
	 * @brief 頂点のGPUコピー・再生成リクエスト状態を設定します。
	 */
	void SetIsVertexGenerationRequested(bool _isRequested);

	/**
	 * @brief キャラクターなどが侵入不可能な最大傾斜角（度数法）を取得します。
	 */
	float GetMaxSlopeAngle() const;

};


} /// ONEngine
