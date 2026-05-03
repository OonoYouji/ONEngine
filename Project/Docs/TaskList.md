# ONEngine 開発タスクリスト (2026/05/03)

現在の進捗状況と `RenderingRoadmap.md` に基づいたタスク管理表です。

## ■ 完了済み (Completed)
- [x] **アセットシステムの抽象化と統合**: `IAsset`, `AssetHandle`, `AssetRegistry` による統一管理基盤の実装。
- [x] **ECS 基礎基盤の実装**: Sparse Set ベース의 Registry と View システム。
- [x] **レンダリングコアの構築**: DirectX 12 基盤、コマンドキュー、スワップチェーン。
- [x] **インスタンスバッチング**: `Renderer` による `InstanceData` の集約と一括描画。
- [x] **Bindless Texture 基礎**: 大規模 Descriptor Heap によるテクスチャ管理とシェーダー参照。
- [x] **データ構造の自動生成**: `schema.yaml` から C++/HLSL 構造体の一斉生成 (`codegen.py`)。
- [x] **アセット管理システム**: マテリアル (`.mat`)、モデル (`.obj`)、テクスチャのロード・管理。
- [x] **Pipeline の外部ファイル化**: PSO 設定を JSON から動的にロードする仕組み。
- [x] **Shader Reflection の統合**: HLSL コンパイル結果からルートシグネチャを自動構築。

## ■ 現在進行中 (In Progress)
- [ ] **ループの正常化**: フレームレート非依存の更新処理（デルタタイム）の実装。
- [ ] **C# スクリプトホスティングの統合**: CoreCLR の組み込みと Interop 層の構築。
- [ ] **Texture Bindless の完全統合**: 全アセットの種類を問わず Descriptor Heap を一元管理。


## ■ 今後の予定 (Future Tasks)

### フェーズ 1: 基盤強化 (短期)
- [ ] **Shader Reflection の完全統合**: 手動での Descriptor レイアウト定義を撤廃。
- [ ] **Texture Bindless の完全統合**: 全アセットの種類を問わず Descriptor Heap を一元管理。
- [ ] **C# スクリプトホスティングの統合**: `main.cpp` への CoreCLR 組み込みと Interop の実証。

### フェーズ 2: マテリアル・アセット洗練 (中期)
- [ ] **Material-to-Shader 選択システム**: マテリアルごとに適用するシェーダーを柔軟に変更。
- [ ] **meta ファイル機能**: インポート時の設定（テクスチャ圧縮等）を外部ファイルで保持。
- [ ] **Clustered Forward の最適化**: GPU 上でのライトカリング実装。

### フェーズ 3: アニメーション・ジオメトリ (中期)
- [ ] **GPU Animation (Skinning)**: Compute Shader によるボーン変形の実装。
- [ ] **Geometry Pool GC**: 使用されなくなったメッシュデータのメモリ回収機構。
- [ ] **GPU-Driven Culling**: HZB を用いたオクルージョンカリング。

### フェーズ 4: ポストエフェクト・ビジュアル (長期)
- [ ] **影の高度化 (CSM)**: カスケードシャドウマップによる高品質な影。
- [ ] **アニメ調アウトラインの洗練**: 深度・法線を用いた高品質な外形線。
- [ ] **GPU パーティクルシステム**: 大量のパーティクルを ECS 経由で制御。

### フェーズ 5: プロファイリング・安定化
- [ ] **GPU Profiler**: パスごとの処理時間を計測・可視化。
- [ ] **マルチビュー対応**: ゲーム画面とデバッグ画面の分離。
