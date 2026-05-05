# Hybrid ECS Engine - 次のステップと開発ガイド

設計書 (v6.1) に基づく、C++ ECS コアと C# スクリプト層の統合基盤の構築が完了しました。全ての主要コンポーネントは `Temp/Temp/Temp/` ディレクトリに集約されています。

## 1. 構築されたアーキテクチャの概要

- **Single Source of Truth (SSOT):** `schema.yaml` にコンポーネントを定義。
- **自動同期パイプライン:** `codegen.py` が C++ / C# / HLSL のコードを同時生成。
- **高速 Interop:** C# 側で C++ のページポインタを直接参照し、O(1) でデータを検索。
- **ゼロコピー・イテレーション:** チャンク単位で `Span<T>` を取得し、メモリ転送なしで処理。

## 2. 開発ワークフロー

新しいコンポーネントを追加・変更する際の手順は以下の通りです：

1.  **`schema.yaml` を更新:**
    フィールド名、型、および `is_gpu_buffer` 等のフラグを設定します。
2.  **ジェネレータを実行:**
    `Temp/Temp/Temp/` 内で `python codegen.py` を実行。
3.  **C++ DLL のビルド:**
    Visual Studio で `Temp.vcxproj` を開き、ビルドして `Temp.dll` を生成します。
4.  **C# 側の実装:**
    自動生成された拡張メソッド（例：`AddPosition`）を使用してロジックを記述します。

## 3. 主要ファイル構成

- **`Engine_*.md`**: 領域ごとの詳細設計書 (Core, Interop, Tooling)。
- **`SparseSet.h` / `EcsWorld.h`**: アドレス固定チャンクとページング sparse array のコア。
- **`EcsInterop.cpp`**: 汎用的な Thin C-API 実装。
- **`GeneratedComponents.*`**: 自動生成された三位一体のコード。
- **`EcsApp.cs`**: C# 側の高速ルックアップとイテレーションの実装例。

## 4. 今後の拡張ロードマップ

1.  **DX12 レンダラの実装:** `GeneratedComponents.hlsli` を使い、`Transform` チャンクを直接 GPU へ Upload するパスの構築。
2.  **CoreCLR ホスティング:** C++ 側から C# の `UpdateAll` を呼び出す起動シーケンスの統合。
3.  **Cleanup フェーズの実装:** `Destroy Queue` を介した `GCHandle.Free()` の確実な実行。

---
本エンジンは、Windows/DirectX 12 に特化した極めて高いパフォーマンスポテンシャルを持っています。
設計に基づいた各フェーズの「ロック（確定）」と「自動化」が完了しているため、迅速な機能追加が可能です。
