#pragma once


/// std
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Mesh/Model.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// ///////////////////////////////////////////////////
/// CustomMeshRenderer
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class CustomMeshRenderer
 * @brief 動的にプログラムから生成する頂点データ（Vertex）およびインデックスデータをもとに、実行時にカスタムメッシュを構築して描画するレンダラーコンポーネントクラス
 */
class CustomMeshRenderer final : public IRenderComponent {
public:

	struct Vertex {
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
	};

	using CustomMesh = Mesh<Vertex>;

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	CustomMeshRenderer();

	/**
	 * @brief デストラクタ
	 */
	~CustomMeshRenderer();

	/**
	 * @brief 頂点・インデックス情報から、GPU側に必要なメッシュ用バッファ（Vertex/IndexBuffer）を再構築（生成）します。
	 * @param _pDxDevice 構築に使用するDxDevice
	 */
	void MeshRecreate(class DxDevice* _pDxDevice);

	/**
	 * @brief CPU側のメモリ上にある最新の頂点配列データを、GPUのアップロードバッファへコピー転送します。
	 */
	void VertexMemcpy();

private:
	/// ===================================================
	/// private : objects
	/// ====================================================

	CustomMesh mesh_;
	std::string texturePath_ = "Packages/Textures/uvChecker.png";
	bool isVisible_ = true; ///< 描画するかどうか
	bool isBufferRecreate_ = false; ///< バッファを再作成するかどうか

	GPUMaterial gpuMaterial_;

public:
	/// ====================================================
	/// public : accessor
	/// ====================================================

	/**
	 * @brief 頂点データ配列をセットします。
	 * @param _vertices 頂点データの配列
	 */
	void SetVertices(const std::vector<Vertex>& _vertices);

	/**
	 * @brief 頂点インデックスデータ配列をセットします。
	 * @param _indices インデックス配列
	 */
	void SetIndices(const std::vector<uint32_t>& _indices);

	/**
	 * @brief 描画テクスチャのアセットファイルパスを設定します。
	 * @param _path アセットパス
	 */
	void SetTexturePath(const std::string& _path);

	/**
	 * @brief マテリアルの基本カラー（RGBA）を設定します。
	 */
	void SetColor(const Vector4& _color);

	/**
	 * @brief このカスタムメッシュを描画するかどうかの表示フラグを設定します。
	 */
	void SetIsVisible(bool _isVisible);

	/**
	 * @brief 次回更新時にGPUバッファを強制再作成するかどうかの要求フラグを設定します。
	 */
	void SetIsBufferRecreate(bool _isBufferRecreate);



	/**
	 * @brief 設定されているテクスチャアセットのファイルパスを取得します。
	 */
	const std::string& GetTexturePath() const;

	/**
	 * @brief 設定されているマテリアルカラーを取得します。
	 */
	const Vector4& GetColor() const;

	/**
	 * @brief 内部メッシュオブジェクト（CustomMesh）を取得します。
	 */
	const CustomMesh* GetMesh() const;

	/**
	 * @brief 描画表示フラグを取得します。
	 */
	bool GetIsVisible() const;

	/**
	 * @brief GPUバッファ再作成要求フラグを取得します。
	 */
	bool GetIsBufferRecreate() const;

	/**
	 * @brief GPUバインド用にパックされたマテリアルデータを取得します。
	 */
	const GPUMaterial& GetGpuMaterial();

};



} /// ONEngine
