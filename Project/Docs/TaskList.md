# ONEngine 開発タスクリスト (2026/05/03)

現在の進捗状況と `RenderingRoadmap.md` に基づいたタスク管理表です。

## ■ 完了済み (Completed)
- [x] **アセットシステムの抽象化と統合**: `IAsset`, `AssetHandle`, `AssetRegistry` による統一管理基盤の実装。
- [x] **ECS 基礎基盤の実装**: Sparse Set ベース의 Registry と View システム。
- [x] **レンダリングコアの構築**: DirectX 12 基盤、コマンドキュー、スワップチェーン。
- [x] **インスタンスバッチング**: `Renderer` による `InstanceData` の集約 e 一括描画。
- [x] **Bindless Texture 基礎**: 大規模 Descriptor Heap によるテクスチャ管理とシェーダー参照。
- [x] **データ構造の自動生成**: `schema.yaml` から C++/HLSL 構造体の一斉生成 (`codegen.py`)。
- [x] **アセット管理システム**: マテリアル (`.mat`)、モデル (`.obj`)、テクスチャのロード・管理。
- [x] **Pipeline の外部ファイル化**: PSO 設定を JSON から動的にロードする仕組み。
- [x] **Shader Reflection の統合**: HLSL コンパイル結果からルートシグネチャを自動構築。
- [x] **Extract Phase の実装**: ロジックとレンダリングの分離（Triple Buffering）。
- [x] **Frame Resource (Triple Buffering)**: コマンドアロケータ等のフレーム単位管理。
- [x] **Z-Prepass**: 深度先行描画によるピクセル負荷削減。
- [x] **ループの正常化 (2026/05/04)**: フレームレート非依存の更新処理（デルタタイム）の実装。
- [x] **D3D12MA の導入 (2026/05/04)**: メモリ管理の最適化。
- [x] **.NET 8 スクリプトエンジンの統合 (2026/05/04)**: CoreCLR Hosting による高性能スクリプト基盤。
- [x] **C# ECS Interop (2026/05/04)**: C++ 側のコンポーネントを C# から直接操作する機能。
- [x] **Texture Bindless の完全統合 (2026/05/04)**: 全アセットの種類を問わず Descriptor Heap を一元管理し、描画ループから文字列を排除。
- [x] **C# スクリプト API の洗練 (2026/05/04)**: Unity ライクな `GetComponent` や `transform` アクセスの実装。
- [x] **C# 側からの Entity 生成・破棄 (2026/05/04)**: `Instantiate` / `Destroy` の Interop 実装。
- [x] **ECS 層の最適化 (2026/05/04)**: Sparse Set の実装改善、Entity ID の再利用、View 性能の向上。
- [x] **レンダリングインターフェースのリファクタリング (2026/05/04)**: `RenderContext` の導入、`Renderer` の責任分離。
- [x] **C# ホスト層のクリーンアップ (2026/05/04)**: `EngineHost` の整理、スクリプトライフサイクルの基盤整備。
- [x] **シーン・プリファブの外部ファイル化 (2026/05/04)**: JSON ベースの `.scene`, `.prefab` ファイルによるデータ駆動な構成の実装。
- [x] **C# スクリプト変数同期 (2026/05/04)**: JSON の `variables` フィールドから C# のフィールドへリフレクションを用いた自動適用機能の実装。


### フェーズ 2: レンダリングパイプラインの高度化 (中期)
- [ ] **Clustered Light Culling**: 画面分割による効率的なライト計算。
- [ ] **Geometry Pool の完全統合**: 全モデルの頂点データを単一バッファに集約。
- [ ] **Post-Processing 基盤**: トーンマッピング、ブルームの実装。
- [ ] **アウトライン抽出**: 深度・法線を用いたエッジ描画。

### フェーズ 3: 大量オブジェクトとアニメーション (中期)
- [ ] **GPU-Driven Culling**: Compute Shader と ExecuteIndirect によるカリング。
- [ ] **GPU Animation (Skinning)**: Compute Shader によるボーン変形の実装。
- [ ] **Geometry Pool GC**: 使用されなくなったメッシュデータのメモリ回収機構。

### フェーズ 4: ビジュアル品質の向上 (長期)
- [ ] **影の高度化 (CSM)**: カスケードシャドウマップ。
- [ ] **GPU パーティクルシステム**: ECS 連携の大規模パーティクル。

### フェーズ 5: ツールと安定化
- [ ] **GPU Profiler**: 各パスの負荷計測。
- [ ] **マルチビュー対応**: ゲーム画面とデバッグ画面の完全分離。
