# Hybrid ECS Engine 開発進捗レポート

現在のエンジン設計（v6.1）に基づくタスクの完了状況および未完了タスクの整理です。

## 1. 全体進捗概況

| カテゴリ | 進捗率 | ステータス | 概要 |
| :--- | :--- | :--- | :--- |
| **Core ECS** | 100% | 完了 | チャンクベースの高速ストレージとコマンドバッファが安定動作。 |
| **Interop / Scripting** | 100% | 完了 | NativeAOT によるゼロコピー連携と ImGui ブリッジの実装完了。 |
| **Tooling (Codegen)** | 100% | 完了 | YAML -> C++/C#/HLSL/Interop の全自動生成パイプラインが稼働。 |
| **Rendering (DX12)** | 30% | プロトタイプ | GPUへのデータアップロードパスが実装済み。描画ループへの統合が未完。 |
| **Data / Prefab** | 100% | 完了 | JSONからの自動生成、GCHandle のライフサイクル管理を厳密化。 |

---

## 2. 詳細タスクリスト

### 2.1. Core ECS (C++)
- [x] **Sparse Set ストレージ実装**: ページング方式による O(1) アクセス。
- [x] **アドレス固定チャンク (Address-stable Chunks)**: C# 側からの直接参照を安全にするための 1024 単位の固定メモリ確保。
- [x] **コマンドバッファ**: エンティティの生成・破棄をフレーム末尾に遅延実行。
- [x] **型IDシステム (TypeID)**: 静的な型登録と動的なストレージ生成。
- [x] **削除フック (Destruction Hooks)**: コンポーネント破棄時に C# 側の GCHandle を解放するための通知機構。

### 2.2. Interop & Scripting (C# / C++)
- [x] **Thin C-API (EcsInterop)**: C# から安全に呼び出すための DLL エクスポート層。
- [x] **NativeAOT 統合**: C# ロジックを AOT コンパイルし、C++ から関数ポインタで呼び出し。
- [x] **ホットリロード**: `ScriptHost` による DLL の監視と実行中の再ロード。
- [x] **ゼロコピー・イテレーション**: `Span<T>` を用いた高速なコンポーネント走査。
- [x] **ImGui ブリッジ**: C++ 側の UI 命令を C# から呼び出し可能に。

### 2.3. Tooling / Pipeline
- [x] **Single Source of Truth (schema.yaml)**: コンポーネント定義の一元化。
- [x] **Python Codegen (Automated Interop)**:
    - C++ 構造体 & Interop 登録コードの自動生成。
    - C# クラス/拡張メソッド生成。
    - HLSL 構造体生成。
    - JSON デシリアライザの自動生成。

### 2.5. Data & Application
- [x] **Prefab Loading**: `System.Text.Json` を用いた動的なエンティティ生成。
- [x] **Script Update Loop**: C# 側の `Update()` 関数をバッチで呼び出す仕組み。
- [x] **GCHandle ライフサイクル管理の厳密化**: `Cleanup()` および `Dispose()` での確実な解放。
- [x] **ImGui UI Integration**: `PlayerMoveScript` 等で実際に UI ロジックを記述可能に。


---

## 3. 直近の課題と推奨アクション

1.  **レンダリングループの統合**: `main.cpp` に `EcsRenderer` を組み込み、実際に画面に何か（キューブ等）を出す。
2.  **GCHandle リークチェック**: エンティティの大量生成・破棄を繰り返し、メモリ使用量が安定するかを確認するテストの追加。
3.  **CoreCLR への移行検討**: 現在の NativeAOT はビルドが遅いため、開発効率向上のために `nethost` を使った通常の .NET DLL ロードへの切り替えを検討する。

---
*最終更新: 2026年4月30日*
