# Rendering System Implementation Guidelines

本エンジン (ONEngine) のレンダリングシステムに新しい機能やシェーダーを実装する際の制約とルールを記述します。

## 1. 頂点フェッチと描画 (Manual Vertex Fetching)
本エンジンは `InputLayout` を使用せず、シェーダー内で頂点データを直接取得する **Manual Vertex Fetching** を採用しています。

*   **ルール:** `ExecuteIndirect` または `DrawIndexedInstanced` の `baseVertexLocation` は常に **0** としてください。
*   **理由:** 頂点シェーダー内で `gVertices[inst.vertexOffset + vID]` のように、`InstanceData` に保持された `vertexOffset` を明示的に加算して頂点を特定するためです。
*   **インデックス:** `startIndexLocation` にはモデル固有の `indexOffset` を設定してください。

## 2. インスタンス管理と GPU カリング
描画は原則として `GPUCullingManager` を介した GPU 駆動のカリングパスを通ります。

*   **バッファ構成:** カリング後のインスタンスデータは `outputInstances_` に格納されます。
*   **オフセット:** 各バッチ (モデル/マテリアルの単位) は、バッファ内で `batchIndex * 2048` のオフセット位置にデータを書き込みます。
*   **アラインメント:** `ExecuteIndirect` で使用する `DrawIndexedArguments` は、GPU の書き込み/読み取りの整合性を保つため **32 bytes** アラインメント（16 bytes の倍数）を維持し、末尾にパディングを入れてください。

## 3. バインドレス・テクスチャ (Bindless Textures)
テクスチャのバインドに `DescriptorTable` を個別に切り替える方式は使用しません。

*   **ルール:** シェーダー側では `Texture2D gTextures[] : register(t0, space1);` として宣言し、`NonUniformResourceIndex(inst.textureIndex)` を用いてアクセスしてください。
*   **Space:** テクスチャは `space1` に固定されています。

## 4. リソースバリアと状態管理
`GPUCullingManager` や `Renderer` 内でのパス間移動時には、適切なリソースバリアが必要です。

*   **カウンタバッファ (`drawArgsBuffer_`):**
    *   カリングパス (CS): `UNORDERED_ACCESS`
    *   コマンド構築パス (CS): `NON_PIXEL_SHADER_RESOURCE` (SRVとして参照)
    *   描画パス (ExecuteIndirect): `INDIRECT_ARGUMENT`
*   **原則:** 各メソッドは、実行開始時に必要な状態へ遷移させ、完了時に後続パスが期待するデフォルト状態（または元の状態）へ戻す責務を持ちます。

## 5. データ定義の同期 (SSOT)
構造体を追加・変更する場合は、必ず `schema.yaml` を編集し、`codegen.py` を実行してください。

*   **対象:** C++ 構造体、C# 構造体、HLSL (`Schema.hlsli`)。
*   **注意:** 手動でヘッダーを書き換えると、Interop 層や定数バッファのパディング位置がズレて、描画の破綻やクラッシュの原因になります。

## 6. シェーダーの登録
新しいシェーダーを実装した際は、以下の 2 ステップが必要です。

1.  `Assets/Pipelines/*.json` にパイプライン定義（テンプレート名、CS/VS/PS のパス等）を作成する。
2.  `Application.cpp` の初期化フローで `sm.LoadPipelineAsset("...")` を呼び出し、システムに登録する。

---
*Created: 2026/05/08*
