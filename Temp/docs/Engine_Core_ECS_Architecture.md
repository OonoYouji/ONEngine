# エンジン設計書：コア基盤とECSアーキテクチャ (Engine_Core_ECS_Architecture)

## 1. ECS メモリ構造 (Memory Layout)
### 1.1 Chunked Sparse Set
* **Dense Array (実データ):** `std::vector<std::unique_ptr<T[]>>` による管理。
    * チャンクサイズは 1024 固定。
    * **アドレス固定化:** `unique_ptr` を用いることで、配列拡張時も既存要素のメモリアドレスを固定。C#側の `Span<T>` 参照を安全に維持する。
* **Sparse Array (インデックス):** ページング方式（Paged Sparse Array）。
    * 1ページ 4096 要素。アクセスは `Index >> 12` と `Index & 4095` で行う。

### 1.2 エンティティ管理
* **Generational Index:** 32-bit Index + 32-bit Generation の 64-bit ID。
* **安全性:** ABA問題を回避し、再利用されたIDへの誤アクセスを防止。

## 2. Windows / DX12 同期パイプライン
### 2.1 厳密なフレームフェーズ (Frame Timeline)
ECSの動的メモリ移動（Swap and Pop）とDX12の静的参照の競合を防ぐため、以下の順序を遵守する。
1. **Update (C#/C++):** ロジック実行。構造変更（生成・破棄）は `CommandBuffer` に蓄積。
2. **Submit ECS (C++):** コマンドを実行し、メモリ配置を**完全確定（ロック）**する。
3. **GPU Upload (C++):** 確定したデータを GPU のカレントフレームバッファへ転送。
4. **Record Render (C++):** 確定したインデックスを用い DX12 コマンドリストを構築。
5. **Submit GPU (C++):** GPUへ命令送信 (`ExecuteCommandLists`)。
6. **Cleanup (C#):** 破棄されたマネージドリソース（GCHandle等）の解放。

### 2.2 マルチバッファリング (Ring Buffer)
* **設計:** `is_gpu_buffer: true` のデータは、`NUM_FRAMES_IN_FLIGHT` 分の配列として確保。
* **競合回避:** CPUは常に「GPUが現在参照していないインデックス」のバッファへ書き込む。

### 2.3 リソースバリア
* **管理責任:** C++側の `RenderSystem` が一括管理。C#側や各システムはバリアを意識せず、Upload時にC++コアがバッチ発行する。

## 3. 遅延実行とフック
* **Strict Deferred Destruction:** `Update` フェーズ中の即時破棄を禁止。
* **Opt-in Destruct Hooks:** YAMLで `requires_dispose: true` 指定時のみ、削除コールバックを登録。
