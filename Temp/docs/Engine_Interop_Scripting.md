# エンジン設計書：C#連携とスクリプト通信 (Engine_Interop_Scripting)

## 1. C# 実行環境
* **Runtime:** .NET 8+ / CoreCLR Hosting。Windows専用。
* **Interop原則:** Thin C-API (C++側は汎用関数のみ) + Thick Wrapper (C#側で型安全にラップ)。

## 2. バッチディスパッチ (Update最適化)
* **課題:** 1エンティティごとの P/Invoke はコスト過大。
* **解決策:** フレームあたり1回のみ、C++からC#の `UpdateAll(ChunkData* chunks)` を呼び出す。
* **C#側の処理:** 1. 渡された `ScriptComponent` のチャンクを `foreach` で回す。
    2. 各要素の `GCHandle` ポインタから `GameScript` インスタンスを取得。
    3. C#の世界の中で各 `Update()` を実行。

## 3. 高速メモリアクセス (SparseSetLookup)
* **O(1) 逆引き:** C++のページ配列（`int**`）をC#に直接渡す。
* **実装:** C#側で `SparseSetLookup` 構造体を定義。ビットシフト演算により、透過的にページとオフセットを計算。
* **ゼロコピー:** チャンクの実データは `Span<T>` でラップして直接読み書き。

## 4. ライフサイクルとリソース管理
* **ScriptComponent:** C++側に C# インスタンスを指すポインタを保持。
* **GCHandle解放:** 1. C++でエンティティ破棄時、IDを「破棄予約キュー」に追加。
    2. フレーム終端の `Cleanup` フェーズでC#へ一括通知。
    3. C#側で `GCHandle.Free()` を実行。

## 5. 自動生成ラッパー
* Pythonジェネレータが、汎用C-APIを隠蔽する `partial class` や拡張メソッドを生成。
* 例: `entity.AddPosition(pos)` という型安全な呼び出しを可能にする。
