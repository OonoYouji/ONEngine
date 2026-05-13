# ONEngine Implementation Rules (Gemini CLI)

このドキュメントは、Gemini CLI が ONEngine の開発を行う際の必須ルールを規定したものです。

## 1. Schema-First Development
*   コンポーネント、定数バッファ、構造化バッファの定義・変更を行う際は、必ず `Project/Tools/Schema/schema.yaml` を SSOT (Single Source of Truth) とすること。
*   変更後は `Project/Tools/RunCodeGen.bat`（または `codegen.py`）を実行し、C++ / C# / HLSL / UI コードを全自動生成すること。手動でのヘッダー書き換えは原則禁止。

## 2. Undo/Redo & Command Implementation
*   エディターにおける全ての状態変更（Inspector でのパラメータ編集、Scene View でのギズモ操作等）は、必ず `CommandHistory` を介して実行すること。
*   汎用的な値変更には `ChangeComponentCommand` を使用し、必要に応じて新しい `ICommand` 実装を追加すること。

## 3. Memory Safety & ECS
*   ECS コンポーネントは Sparse Set によるアドレス固定メモリで管理されている。C# 側からの Zero-Copy 参照を破壊しないよう、エディター上での削除は論理削除（`IsEditorDeleted` フラグ等）を検討すること。

## 4. Rendering Standards
*   Manual Vertex Fetching: `InputLayout` は使用せず、頂点シェーダー内で頂点データを直接取得する。
*   Bindless Texture: `space1` にバインドされたテクスチャ配列をインデックスで参照する。
*   新機能の実装時は `Project/Docs/Guidelines/RenderingGuidelines.md` を遵守すること。

## 5. Build & Environment Integrity (Mandatory)
*   **ファイル追加・削除時のプロジェクト更新**: 
    新しいソースファイルを追加、または既存ファイルを削除・移動した場合は、必ず `Project/GenerateProject.ps1` または `Project/premake5.exe` を実行して Visual Studio プロジェクトファイルを更新すること。
*   **ビルド確認の徹底**: 
    新規実装やリファクタリングを行った後は、必ずビルド（`msbuild` または `devenv`）を実行し、**ビルドエラーがない状態**であることを確認してからタスクを完了とすること。

## 6. Minimal Changes to Existing Code
*   新規機能の実装やシステムの導入を行う際は、既存のソースファイルへの変更を**必要最低限**に留めること。
*   既存のコードベースを最大限尊重し、不必要なリファクタリングや広範囲な書き換えを避けることで、既存機能の破壊やデグレードを防止すること。

## 7. Documentation
*   重要な設計変更や新システムの導入時は、`Project/Docs/Design/` 以下の関連ドキュメントを更新すること。
