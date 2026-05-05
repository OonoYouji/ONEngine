# エンジン設計書：データ管理と自動化ツール (Engine_Data_Tooling)

## 1. YAML SSOT (Single Source of Truth)
* **役割:** すべてのコンポーネント定義、初期値、メタデータを一元管理。
* **メタデータ項目:**
    * `description`: ドキュメント用。
    * `default`: C#側の初期値。
    * `is_gpu_buffer`: true の場合、HLSLアライメント計算とマルチバッファリングを適用。
    * `requires_dispose`: true の場合、C++側に削除フックを生成。

## 2. Python ジェネレータ
* **生成物:**
    1. `GeneratedComponents.h` (C++構造体)
    2. `GeneratedComponents.cs` (C#構造体 + Layout属性 + 拡張メソッド)
    3. `GeneratedComponents.hlsli` (HLSL構造体)
    4. `Interop.cpp/cs` (C-APIのエクスポート/インポート定義)
* **Watchモード:** YAML保存を検知し、0.1秒以内に再生成。IDE (Visual Studio) との即時同期を実現。

## 3. HLSL アライメント自動化
* **パディング計算:** DX12 (HLSL) の 16バイトアライメント規則に基づき、Python側でオフセットを計算。
* **ダミー変数:** 不足分に `float _pad0;` 等を自動挿入し、C++/C#/HLSLのメモリレイアウトを完全に一致させる。
* **C#属性:** `[StructLayout(LayoutKind.Explicit)]` と `[FieldOffset(x)]` を自動付与。

## 4. シリアライズとマージ (実装済み)
* **Cascade Merge 優先順位:**
    1. `Base` (YAML定義のデフォルト値) - **実装済み**: Codegenにより C# の `ComponentRegistry` に埋め込み。
    2. `Prefab JSON` (テンプレート値) - **実装済み**: `PrefabLoader.Load` により JSON からエンティティを生成。
    3. `Scene JSON` (インスタンス固有のオーバーライド) - *予定*
* **マージルール:** 
    * プリミティブ/構造体: JSONに値があれば採用、なければ YAML のデフォルト値を使用。
    * リスト/配列: **完全置換 (Replace)**。要素の混入を防ぐための意図的な制限。
