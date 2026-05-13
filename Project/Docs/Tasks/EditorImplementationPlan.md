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
- [x] **EditorCamera の実装** (2026/05/11)
- [x] **UI フックの実装** (Editor プロジェクトから UI を描画するための仕組み) (2026/05/13)

## 3. シーンビューと G-Buffer 連携 (Phase 3)
エディターの核となる描画・選択機能を実装します。

- [x] **Scene View ウィンドウ** (2026/05/11)
    - レンダリング結果を ImGui のウィンドウ内に描画。
- [ ] **マウスピッキング (RT2 連携)**
    - クリック位置の `RT2` (EntityID) を GPU から Readback。
    - 選択された Entity の特定とハイライト。
- [ ] **ImGuizmo の導入**
    - 選択した Entity をシーン上で直接動かすためのギズモを表示。

## 4. 各種ウィンドウの実装 (Phase 4)
設計書に基づいた UI モジュールの実装。

- [x] **Hierarchy ウィンドウ** (2026/05/11)
    - `RelationshipComponent` を用いたツリー表示。
- [x] **Inspector ウィンドウ** (2026/05/13)
    - 選択 Entity のコンポーネント一覧表示。
    - `schema.yaml` に基づく **UI コードの自動生成** 実装済み。
- [ ] **Project Browser**
    - `Assets/` ディレクトリのファイル走査と表示。

## 5. Undo/Redo & シリアライズ (Phase 5)
編集作業の安定性と永続化。

- [x] **Command パターンの実装** (2026/05/12)
    - `ICommand` インターフェースの定義。
    - `ChangeComponentCommand` による汎用的な値変更の実装。
- [x] **Undo/Redo システム** (2026/05/12)
    - `CommandHistory` による履歴管理。
- [ ] **Value Snapshot (Soft Delete)**
    - メモリ安全な論理削除の実装。
- [x] **シーン保存・読み込み** (2026/05/13)
    - 自動生成された `to_json` / `from_json` を用いたシリアライズ。

---
*Created: 2026/05/11*
