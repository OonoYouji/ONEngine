#pragma once

/// std
#include <optional>

/// engine
#include "../IAsset.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/Data/UVTransform.h"


namespace ONEngine::Asset {


/// ShowGuiMaterialように前方宣言
class Material;

/**
 * @brief 初期値が設定されたデフォルトのマテリアルを生成します。
 * @return 生成されたMaterialオブジェクト
 */
Material GenerateMaterial();

/**
 * @brief マテリアル情報（JSON形式）をファイルとして新規生成します。
 * @param _filepath 生成するマテリアルファイルのパス
 * @param _material ソースにするマテリアルのポインタ（nullptrの場合はデフォルト値で作成）
 */
void GenerateMaterialFile(const std::string& _filepath, Material* _material);

/**
 * @brief jsonオブジェクトからMaterialオブジェクトへデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _material 復元先マテリアルの参照
 */
void from_json(const nlohmann::json& _j, Material& _material);

/**
 * @brief Materialオブジェクトからjsonオブジェクトへシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _material 変換元マテリアル
 */
void to_json(nlohmann::json& _j, const Material& _material);

/**
 * @class Material
 * @brief メッシュ描画時のカラー、テクスチャ、UV変形パラメータ等を制御するマテリアルアセットクラス
 */
class Material final : public IAsset {
	/// friend functions
	friend void from_json(const nlohmann::json& _j, Material& _material);
	friend void to_json(nlohmann::json& _j, const Material& _material);

public:

	/**
	 * @struct MetaData
	 * @brief マテリアルアセット固有のメタデータ
	 */
	struct MetaData {
		std::string useShader;   ///< 使用するシェーダー名またはパス
		Vector4 albedoColor;     ///< アルベド（ベース）カラー
		Guid albedoTextureGuid;  ///< アルベドテクスチャのGUID
		Guid normalTextureGuid;  ///< 法線テクスチャのGUID
	};


public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	/**
	 * @brief コンストラクタ。デフォルトのカラー値やUV変換パラメータを初期化します。
	 */
	Material();

	/**
	 * @brief デストラクタ
	 */
	~Material();


	/**
	 * @brief ベース（アルベド）テクスチャが設定されているか判定します。
	 * @return 設定されている場合はtrue
	 */
	bool HasBaseTexture() const;

	/**
	 * @brief ベース（アルベド）テクスチャのGUIDを取得します。
	 * @return テクスチャGUIDの定数参照（未設定時は Guid::kInvalid）
	 */
	const Guid& GetBaseTextureGuid() const;

	/**
	 * @brief ベース（アルベド）テクスチャのGUIDを設定します。
	 * @param _guid 設定するテクスチャのGuid
	 */
	void SetBaseTextureGuid(const Guid& _guid);


	/**
	 * @brief 法線（ノーマル）テクスチャが設定されているか判定します。
	 * @return 設定されている場合はtrue
	 */
	bool HasNormalTexture() const;

	/**
	 * @brief 法線（ノーマル）テクスチャのGUIDを取得します。
	 * @return 法線テクスチャのGUID
	 */
	const Guid& GetNormalTextureGuid() const;

	/**
	 * @brief 法線（ノーマル）テクスチャのGUIDを設定します。
	 * @param _guid 設定するテクスチャのGuid
	 */
	void SetNormalTextureGuid(const Guid& _guid);

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	std::optional<Guid> baseTextureGuid_;   /// ベーステクスチャのGUID
	std::optional<Guid> normalTextureGuid_; /// 法線テクスチャのGUID


public:
	/// ==================================================
	/// public : objects
	/// ==================================================

	Vector4             baseColor;
	uint32_t            postEffectFlags;
	UVTransform         uvTransform;
};


} /// namespace ONEngine::Asset
