#include "CustomMeshRenderer.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

using namespace ONEngine;

CustomMeshRenderer::CustomMeshRenderer() {
	gpuMaterial_.baseColor = Vector4::White;
	gpuMaterial_.postEffectFlags = PostEffectFlags_Lighting;
}

CustomMeshRenderer::~CustomMeshRenderer() {}

/**
 * @brief 頂点・インデックス情報から、GPU側に必要なメッシュ用バッファ（Vertex/IndexBuffer）を再構築（生成）します。
 */
void CustomMeshRenderer::MeshRecreate(DxDevice* _pDxDevice) {
	mesh_.CreateBuffer(_pDxDevice);
	mesh_.VertexBufferMapping();
	mesh_.IndexBufferMapping();
}

/**
 * @brief CPU側のメモリ上にある最新の頂点配列データを、GPUのアップロードバッファへコピー転送します。
 */
void CustomMeshRenderer::VertexMemcpy() {
	mesh_.MemcpyVertexData();
}

/**
 * @brief 頂点データ配列をセットします。
 */
void CustomMeshRenderer::SetVertices(const std::vector<CustomMeshRenderer::Vertex>& _vertices) {
	mesh_.SetVertices(_vertices);
}

/**
 * @brief 頂点インデックスデータ配列をセットします。
 */
void CustomMeshRenderer::SetIndices(const std::vector<uint32_t>& _indices) {
	mesh_.SetIndices(_indices);
}

/**
 * @brief 描画テクスチャのアセットファイルパスを設定します。
 */
void CustomMeshRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

/**
 * @brief マテリアルの基本カラー（RGBA）を設定します。
 */
void CustomMeshRenderer::SetColor(const Vector4& _color) {
	gpuMaterial_.baseColor = _color;
}

/**
 * @brief このカスタムメッシュを描画するかどうかの表示フラグを設定します。
 */
void CustomMeshRenderer::SetIsVisible(bool _isVisible) {
	isVisible_ = _isVisible;
}

/**
 * @brief 次回更新時にGPUバッファを強制再作成するかどうかの要求フラグを設定します。
 */
void CustomMeshRenderer::SetIsBufferRecreate(bool _isBufferRecreate) {
	isBufferRecreate_ = _isBufferRecreate;
}

/**
 * @brief 設定されているテクスチャアセットのファイルパスを取得します。
 */
const std::string& CustomMeshRenderer::GetTexturePath() const {
	return texturePath_;
}

/**
 * @brief 設定されているマテリアルカラーを取得します。
 */
const Vector4& CustomMeshRenderer::GetColor() const {
	return gpuMaterial_.baseColor;
}

/**
 * @brief 内部メッシュオブジェクト（CustomMesh）を取得します。
 */
const CustomMeshRenderer::CustomMesh* CustomMeshRenderer::GetMesh() const {
	return &mesh_;
}

/**
 * @brief 描画表示フラグを取得します。
 */
bool CustomMeshRenderer::GetIsVisible() const {
	return isVisible_;
}

/**
 * @brief GPUバッファ再作成要求フラグを取得します。
 */
bool CustomMeshRenderer::GetIsBufferRecreate() const {
	return isBufferRecreate_;
}

/**
 * @brief GPUバインド用にパックされたマテリアルデータを取得します。
 */
const GPUMaterial& CustomMeshRenderer::GetGpuMaterial() {
	return gpuMaterial_;
}
