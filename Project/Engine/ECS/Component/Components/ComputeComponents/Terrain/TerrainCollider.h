#pragma once

/// std
#include <vector>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Terrain.h"

/// ///////////////////////////////////////////////////
/// 地形のコライダーコンポーネント
/// ///////////////////////////////////////////////////
class TerrainCollider : public IComponent {
	friend class TerrainColliderVertexGenerator;
public:
	/// =========================================
	/// public : methods
	/// =========================================

	TerrainCollider();
	~TerrainCollider() override = default;

	void AttachTerrain();

	/// @brief 地形の頂点情報をコピーする
	void CopyVertices(class DxManager* _dxManager);

	/// @brief 地形の高さを取得する
	/// @param _position 取得したい場所
	/// @return 高さ
	float GetHeight(const Vector3& _position);

	/// @brief 引数座標の勾配を取得する
	/// @param _position ワールド座標
	Vector3 GetGradient(const Vector3& _position);

	/// @brief 地形の内側にいるかどうかを判定する
	/// @param _position 判定したい座標
	/// @return true: 内側 false: 外側
	bool IsInsideTerrain(const Vector3& _position);


private:
	/// =========================================
	/// private : objects
	/// =========================================

	Terrain* pTerrain_;

	std::vector<std::vector<TerrainVertex>> vertices_;

	bool isVertexGenerationRequested_;

	bool isCreated_;


public:
	/// =========================================
	/// public : accessor
	/// =========================================

	Terrain* GetTerrain() const;

	const std::vector<std::vector<TerrainVertex>>& GetVertices() const;
	std::vector<std::vector<TerrainVertex>>& GetVertices();

	bool GetIsCreated() const;


	void SetIsVertexGenerationRequested(bool _isRequested);

};



namespace COMP_DEBUG {
	void TerrainColliderDebug(TerrainCollider* _collider);
}

void from_json(const nlohmann::json& _j, TerrainCollider& _c);
void to_json(nlohmann::json& _j, const TerrainCollider& _c);