#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// interface
#include "Engine/ECS/Component/Components/Interface/IComponent.h"

/// ////////////////////////////////////////////////////////
/// 草のインスタンス情報 (シェーダーで利用)
/// ////////////////////////////////////////////////////////
struct GrassInstance {
	Vector3 position;
	Vector3 tangent;
	float scale;
	float random01;
};


/// 下の関数で使うための前方宣言
class GrassField;

/// ////////////////////////////////////////////////////////
/// json変換
/// ////////////////////////////////////////////////////////
void to_json(nlohmann::json& _j, const GrassField& _p);
void from_json(const nlohmann::json& _j, GrassField& _p);


/// ////////////////////////////////////////////////////////
/// Editor
/// ////////////////////////////////////////////////////////
namespace COMP_DEBUG {
	void GrassFieldDebug(GrassField* _grassField);
}


/// ////////////////////////////////////////////////////////
/// Terrainに生やすための草の群クラス
/// ////////////////////////////////////////////////////////
class GrassField : public IComponent {
	/// privateメンバ変数の参照のためにフレンド宣言
	friend void to_json(nlohmann::json& _j, const GrassField& _p);
	friend void from_json(const nlohmann::json& _j, GrassField& _p);
	friend void COMP_DEBUG::GrassFieldDebug(GrassField* _grassField);
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassField();
	~GrassField();

	/// 草のバッファを初期化する
	void Initialize(
		uint32_t _maxBladeCount,
		DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap
	);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- buffer ----- ///
	StructuredBuffer<GrassInstance> rwGrassInstanceBuffer_;

	/// ----- parameters ----- ///
	uint32_t maxGrassCount_; ///< 最大草の本数
	std::string distributionTexturePath_; ///< 草の配置に使うテクスチャのパス

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// 草のインスタンスバッファの取得
	StructuredBuffer<GrassInstance>& GetRwGrassInstanceBuffer();

	/// 最大草の本数の取得
	uint32_t GetMaxGrassCount() const;

};



