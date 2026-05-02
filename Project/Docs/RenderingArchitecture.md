# Hybrid ECS Engine: Technical Specification (v3.2)

## 1. システム基盤 (System Foundation)
C++による高性能なレンダリング・ECSコアと、C# (.NET 8) による柔軟なスクリプト層を組み合わせたハイブリッドアーキテクチャである。

*   **SSOT (Single Source of Truth):** `schema.yaml` に定義されたデータ構造を元に、C++、C#、HLSL の構造体および Interop 層を `codegen.py` で自動生成する[cite: 1]。
*   **Zero-Copy Interop:** C++ 側の SparseSet メモリを C# からポインタで直接参照し、マネージド・アンマネージド間のコピーコストを排除する[cite: 1]。
*   **CoreCLR Hosting:** C++ 側から .NET ランタイムを起動し、JIT実行による高速なスクリプト実行とホットリロードを実現する[cite: 1]。

## 2. ECS & データ同期 (ECS & Thread Safety)
*   **SparseSet Storage:** コンポーネントは密なエンティティ配列（Dense Array）で保持され、キャッシュ効率を最大化する[cite: 1]。
*   **Extract Phase (Snapshot):** 
    *   C# の `Update()` 完了後、レンダリング用の `StructuredBuffer` へデータを一括コピーする[cite: 1]。
    *   **Double Buffering:** レンダラが参照する「抽出済みデータ」を多重化し、C# 側のロジック更新とのデータレースを防止する。

## 3. DirectX 12 管理層 (DX12 Management)
*   **Frame Resource (Triple Buffering):** コマンドアロケータ、定数バッファ、フェンス値をフレーム単位でパッケージ化し、CPU/GPUの非同期実行を維持する。
*   **D3D12MA:** メモリ管理に `D3D12 Memory Allocator` を採用し、動的なメモリ確保・解放を高速化する。
*   **Deferred Deleter:** GPU のフェンス完了を確認してからリソースを破棄する遅延削除キューにより、実行中のリソース破棄を安全に行う。
*   **Descriptor Stack:** 巨大な単一の Descriptor Heap を確保し、Bindless インデックスの空きスロットを管理する。

## 4. レンダリングパイプライン (Clustered Forward)
大量の動的ライトと、アニメ調を含む多彩なマテリアル表現を両立するモダンなパイプライン。

1.  **Z-Prepass:** 深度を先行描画し、後続パスのピクセル負荷を削減する。
2.  **GPU-Driven Culling:** 
    *   Compute Shader によるフラスタムカリング。
    *   **Atomic Counter:** 生き残ったインスタンス数を GPU 上で `InterlockedAdd` を用いてカウントし、`ExecuteIndirect` 用の引数バッファを構築する。
3.  **Light Culling:** Zバッファに基づき画面を 3D クラスタに分割。各ピクセルに影響するライトリストを Compute Shader で生成する。
4.  **Main Pass (Bindless & Manual Fetch):** 
    *   **Manual Vertex Fetching:** `InputLayout` を使用せず、シェーダー内で頂点バッファを手動取得する。
    *   **Bindless Material:** マテリアルインデックスに基づき、テクスチャやパラメータを動的に参照する。
5.  **Post-Processing:** HDRトーンマッピング、ブルーム、および深度・法線情報を用いたアウトライン抽出。

## 5. アセットシステム & ホットリロード (Asset & Reload)
*   **GUID-Based Asset Registry:** 全アセットを一意の GUID で管理。C# 側からは `AssetHandle` を通じて操作し、内部的に GPU インデックスへ解決する[cite: 1]。
*   **Geometry Pool:** 
    *   全モデルの頂点を単一の巨大バッファに集約。
    *   モデル更新時は空き領域に追記し、オフセットを更新する。
*   **Shader Reflection:** HLSL から Root Signature を自動生成。
*   **Pipeline Asset:** 外部 YAML ファイルから PSO (Pipeline State Object) を構築。

## 6. スキニング & 頂点構造 (Skinned Mesh)
*   **Multi-Stream Storage:** 座標、法線/UV、ウェイト/ボーンインデックスを別ストリームで管理。
*   **Skinning Equation:** 頂点シェーダー内で最大4ボーンの影響を計算する。
$$V_{skinned} = \sum_{i=0}^{3} (W_i \cdot M_{joint_i}) \cdot V_{local}$$

## 7. マルチビュー管理 (Multi-View)
*   **ViewContext:** Viewport, Scissor, ViewConstantBuffer（カメラ行列）を Scene ごとに分離。
*   **Simultaneous Scenes:** `GameScene` と `DebugScene` を独立した View として同一フレーム内で描画可能[cite: 1]。

---
*Technical Design v3.2 - 2026/05/01*