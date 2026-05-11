# ONEngine Editor Implementation Plan

このドキュメントは、ONEngine エディターの段階的な実装手順を記録したものです。セッションが中断された場合でも、この計画に基づいて作業を再開します。

## 1. プロジェクト構造の再定義 (Phase 1)
現在の `ONEngine` プロジェクト（実行ファイル）を、汎用的な「Engine」ライブラリと、用途別の「Runtime」「Editor」実行ファイルに分離します。

- [x] **premake5.lua の修正** (2026/05/11)
- [x] **Engineプロジェクトの整備** (2026/05/11)

## 2. Editor 基盤の実装 (Phase 2)
エディターの最小構成を構築します。

- [x] **Editor/Main.cpp の作成** (2026/05/11)
- [x] **ImGui (Docking) の統合** (2026/05/11)
- [ ] **EditorCamera の実装**
- [ ] **UI フックの実装** (Editor プロジェクトから UI を描画するための仕組み)

## 3. シーンビューと G-Buffer 連携 (Phase 3)
エディターの核となる描画・選択機能を実装します。

- [ ] **Scene View ウィンドウ**
    - レンダリング結果を ImGui のウィンドウ内に描画。
- [ ] **マウスピッキング (RT2 連携)**
    - クリック位置の `RT2` (EntityID) を GPU から Readback。
    - 選択された Entity の特定とハイライト。
- [ ] **ImGuizmo の導入**
    - 選択した Entity をシーン上で直接動かすためのギズモを表示。

## 4. 各種ウィンドウの実装 (Phase 4)
設計書に基づいた UI モジュールの実装。

- [ ] **Hierarchy ウィンドウ**
    - `RelationshipComponent` を用いたツリー表示。
    - ドラッグ＆ドロップによる階層移動。
- [ ] **Inspector ウィンドウ**
    - 選択 Entity のコンポーネント一覧表示。
    - `schema.yaml` に基づく自動生成 UI または C# リフレクション連携。
- [ ] **Project Browser**
    - `Assets/` ディレクトリのファイル走査と表示。

## 5. Undo/Redo & シリアライズ (Phase 5)
編集作業の安定性と永続化。

- [ ] **Command パターンの実装**
    - `ICommand` インターフェースの定義。
    - `HierarchyChangeCommand`, `TransformChangeCommand` 等の実装。
- [ ] **Value Snapshot (Soft Delete)**
    - メモリ安全な論理削除の実装。
- [ ] **シーン保存・読み込み**
    - 編集したシーンを JSON で保存。

---
*Created: 2026/05/11*
