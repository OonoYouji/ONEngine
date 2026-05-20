# ONEngine Current Rendering Architecture (feature/new_engine)

このドキュメントでは、現在の ONEngine におけるレンダリングシステムの設計と実装状況を記述します。

## 1. コア・アーキテクチャ
レンダリングシステムは以下の主要なクラスによって構成されています。

### GraphicsEngine
システムの中心となるシングルトンクラスです。
- **役割**: DirectX 12 デバイス、コマンドキュー、スワップチェーン、記述子ヒープ（RTV, SRV, DSV）のライフサイクル管理。
- **リソース**: `MainColorBuffer` (R16G16B16A16_FLOAT), `NormalBuffer`, `IDBuffer`, `FinalColorBuffer` を保持しており、Deferred 寄りの構成を取っています。

### Renderer
描画リクエストの収集と実行を担当します。
- **描画パス**: 現在は `Render` 関数によるメインパスが中心で、`ZPrepass` は未実装（空）です。
- **インスタンシング**: `PushRequest` により描画情報を蓄積し、`Extract` で定数バッファへ転送、一括描画を行います。

## 2. リソース管理
- **GeometryPool**: 全ての頂点・インデックスデータを集約して管理するグローバルなバッファプール。
- **DescriptorHeap**: 記述子（Descriptor）を管理。ImGui 用に 0 番を予約するなどの運用が行われています。
- **ShaderManager**: HLSL のコンパイルと PipelineStateObject (PSO) / RootSignature の生成・キャッシュを担当。

## 3. 先進的レンダリング機能
現在の実装には、既にいくつかの高度な機能が組み込まれています。

### GPU Driven Culling (`GPUCullingManager`)
- **手法**: コンピュートシェーダーによる視錐台カリング。
- **実行**: `ExecuteIndirect` を使用した間接描画により、CPU 負荷を抑えた大量のオブジェクト描画を可能にしています。

### Clustered Lighting (`ClusteredLightManager`)
- **手法**: 画面をタイル（グリッド）状に分割し、さらに奥行き（Z）方向でスライスした「クラスター」単位でライトを割り当てます。
- **利点**: 多数のポイントライトを効率的に処理可能です。

## 4. ポストプロセス (`PostProcessSystem`)
- **構成**: 全画面三角形（`Blit.hlsl`）を使用したマルチパス処理。
- **現在のパス**:
    - **Bloom (未完)**: バッファ確保とシェーダーはあるが、ロジックがダミー。
    - **Final Post Process**: ACES トーンマッピング、ガンマ補正、および ID/Normal ベースのアウトライン処理が実装済み。

## 5. 実装上の特徴と制約
- **Mesh Shader 対応**: `README.md` に記載がある通り、Mesh Shader を前提としたアーキテクチャを目指しています。
- **Bindless への布石**: `srvHeap_` が 2048 スロット確保されており、バインドレスなリソースアクセスを意図した設計が見られます。

---

## 調査日
2026年5月20日
Rendering Programmer: Gemini CLI
