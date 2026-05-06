# Hybrid ECS Engine アーキテクチャ設計書 (2026/05/01 版)

## 1. システム概要
本エンジンは、パフォーマンスが要求されるコアシステム（ECS、メモリ管理、レンダリング）を **C++** で、柔軟なゲームロジック（スクリプト）を **C# (.NET 8)** で記述するハイブリッドアーキテクチャを採用している。

### 特徴
- **CoreCLR Hosting:** C++ 側から直接 .NET ランタイムを起動し、JIT実行による高速なスクリプト更新（ホットリロード）を実現。
- **Zero-Copy Interop:** C++ 側のコンポーネントメモリ（Sparse Set）を C# 側からポインタで直接参照。
- **Hybrid Rendering:** Forward 描画の柔軟性と MRT による詳細なポストプロセス制御（アウトライン等）を統合。フォトリアルとアニメ調の混在をサポート。
- **Automated Codegen:** `schema.yaml` を SSOT（単一の信頼源）とし、C++/C#/HLSL/Interop コードを全自動生成。
- **GPU-Driven Systems:** Mesh Shader による描画と Compute Shader による更新を統合したパーティクルエンジン。

---

## 2. コア層 (C++ Core)

### 2.1. ECS ストレージ (`SparseSet<T>`)
- **ページング Sparse Array:** エンティティ ID からデータインデックスへ O(1) でアクセス。
- **アドレス固定チャンク:** 1024 単位の固定メモリブロックでデータを保持。C# 側でポインタをキャッシュしても安全。
- **Add-or-Replace セマンティクス:** 既に存在するコンポーネントへの `add` 要求を自動的に `update` として処理。

### 2.2. レンダリング基盤 (`GraphicsEngine`)
- **Advanced G-Buffer:** Color(HDR), Normal, ID/Flags の 3 ターゲット構成を標準化。
- **Shader Manager:** Shader Reflection によるルートシグネチャの自動構築と、マルチレンダーターゲット構成の自動 PSO キャッシュ管理。
- **Post-Process Pipeline:** ブルーム、選択的アウトライン抽出、ACES トーンマッピング。

---

## 3. スクリプト層 (C# Scripting)

### 3.1. 型 ID 管理の最適化 (`ComponentID<T>`)
- **静的ジェネリックキャッシュ:** `typeof(T)` による型判定を初回アクセス時のみに抑制。
- 実行時はメモリ上の静的フィールド（`uint ID`）へのアクセスのみとなり、定数時間（O(1)）での型 ID 取得を実現。

### 3.2. Unity 互換 API (`GameScript`)
- **プロパティアクセス:** `this.transform` (LocalTransformへのエイリアス) を提供。
- **コンポーネント操作:** Unity と同等の `GetComponent<T>` 記法をサポート。
- **クエリシステム:** `Query<T1, T2, T3>` によるマルチコンポーネントフィルタリング。

---

## 4. データ・ツール層

### 4.1. Codegen パイプライン (`codegen.py`)
- **SSOT (schema.yaml):** レンダリング、物理、ECS 全体のデータ構造を定義。
- **自動生成対象:**
    - C++ 構造体ヘッダー (`Schema.h`)
    - C# 構造体、型 ID マップ
    - HLSL 構造体 (`Schema.hlsli`)

### 4.2. シーン・プリファブシステム
- **JSON シリアライズ:** 階層化された JSON によるエンティティ定義。
- **初期化同期:** `ComponentRegistry` による JSON からの動的コンポーネント復元。

---

## 5. 実行フロー (Main Loop)

1.  **C++:** 起動、.NET ランタイム初期化、グラフィックスデバイス初期化。
2.  **Frame Loop:**
    - **Update Phase:** C# 側でのロジック実行（1024エンティティ/ms 級の性能）。
    - **Extract Phase:** レンダリング用定数バッファ・構造化バッファへのデータ転送。
    - **Render Phase:** カリング、Z-Prepass、Main Pass (G-Buffer)、Post-Processing。
    - **Execute & Present:** GPU 実行とスワップチェーン提示。

---
*設計ドキュメント v9.0 - 2026/05/06*
