# ONEngine エディター設計仕様書 (v1.4)

本ドキュメントは、自作ゲームエンジン「ONEngine」におけるエディターのアーキテクチャ、データ構造、および各種システムの設計全容を記録したものである。本設計は、C++による高速なECSコアと、C#による柔軟なスクリプト層、およびDirectX12レンダリングパイプラインの統合を前提としている。

---

## 1. 基本アーキテクチャ
* **エンジンコア:** C++ (SparseSetによるアドレス固定メモリ管理)
* **スクリプト層:** C# (.NET 8 CoreCLR Hosting)
* **インターオポ:** Zero-Copy Interop (C#からC++メモリへの直接ポインタ参照)
* **UIフレームワーク:** Dear ImGui (Docking Branch)
* **グラフィックスAPI:** DirectX12

---

## 2. マウスピッキングとG-Buffer連携
* **RT2 (ID/Flags) の活用:** エンジン側のハイブリッドレンダリング設計に基づき、G-Bufferの `RT2` には **Runtime EntityID (uint32_t)** を書き込む。
* **マウスピッキング処理:** Scene View上でクリックされた際、GPUから `RT2` をReadbackし、取得したRuntime IDを用いて対象Entityを特定・選択状態にする。シリアライズ等が必要な場合のみ、そこからIDComponentのGUIDへ変換する。

---

## 3. ECSデータ構造 (Data-Oriented設計)
CPUキャッシュライン（64 byte）を意識し、動的割り当て（`std::string`等）を排除。C#側からのZero-Copy参照を安全に行うため、メモリアドレスが変わらない固定チャンク構造を前提とする。

### IDComponent
* **役割:** シリアライズ、Undo/Redo、プレハブ管理のSSOT（単一の信頼源）。
```cpp
struct IDComponent {
    GUID ID; // 128-bit GUID
};
```

### TagComponent (36 byte)
* **役割:** 名前、タグ、有効状態、およびエディター用の削除状態を管理。
```cpp
struct TagComponent {
    std::array<char, 32> Name;
    bool IsActive;
    bool IsEditorDeleted; // 論理削除フラグ（Undo/Redo、Zero-Copy保護用）
};
```

### RelationshipComponent (16 byte)
* **役割:** 双方向連結リスト（Intrusive Linked List）による階層構造。
```cpp
struct RelationshipComponent {
    EntityID Parent, FirstChild, PrevSibling, NextSibling;
};
```

### TransformComponent
* **役割:** 座標データと更新フラグ（計算順序制御用）。
```cpp
struct TransformComponent {
    bool IsDirty; // 親の変更検知フラグ
    float3 LocalPosition, LocalRotation, LocalScale;
    matrix WorldMatrix;
};
```

---

## 4. Hierarchy API (階層操作システム)
`HierarchySystem` 静的クラスを介して操作を行い、リンクの整合性と循環参照を保証する。コンポーネントへの直接書き込みは禁止。

* **IsDescendantOf:** 循環参照（親を自分の子にする等）を防止するチェック機能。
* **Detach:** 安全に親から切り離し、ルート階層へ移動させる。
* **AttachTo:** 指定した親の「最後の子」として追加。
* **InsertAfter:** 指定した兄弟の直後に挿入。更新・描画順序の制御に必須。
* **InsertAsFirst:** 指定した親の「先頭の子」として挿入。
> **注意:** 階層操作後は、必ず対象の `TransformComponent.IsDirty` を `true` にすること。

---

## 5. ウィンドウモジュールとイベントシステム
エディターUIは疎結合に設計し、`EditorEventSystem`（Observerパターン）を介して通信する。

* **通信プロトコル:**
  * `OnEntitySelected(EntityID)`
  * `OnEntityHierarchyChanged()`
  * `OnComponentChanged(EntityID, ComponentType)`
* **各ウィンドウの役割:**
  * **Scene View:** オフスクリーンレンダリング結果の描画、Editor Cameraの操作、および `ImGuizmo` を用いたTransformの直接編集。
  * **Hierarchy:** ツリー構造の再帰的描画。ドラッグ＆ドロップによる `HierarchySystem` の呼び出し（階層移動）。
  * **Inspector:** 選択Entityのパラメータ編集と、コンポーネントのアタッチ/デタッチ。
  * **Project:** `<filesystem>` を用いたアセットエクスプローラー。ドラッグ＆ドロップでのリソース割り当て機能。

---

## 6. Undo/Redo & メモリ安全性 (Zero-Copy対応)
* **論理削除 (Soft Delete):** エディター上の削除は `IsEditorDeleted = true` とするのみ。Runtime ID とメモリアドレスを維持し、C#のポインタを保護する。物理削除はシーン保存時に遅延実行。
* **メモリ直接上書き (Value Snapshot):** 値の変更Undoは、対象構造体のバイナリコピーを保存し、`memcpy` 等で直接メモリを書き戻す（JSON不要で高速）。
* **コマンドパターン:** 全ての操作を `ICommand` を継承したクラスとして実装し、履歴スタックで管理する。

---

## 7. エディターUIラッパー (Command Bridge)
ImGuiの即時モードとコマンドパターンの橋渡しを行う `EditorUI` 名前空間を設計。
* **Activated:** `IsItemActivated()` で編集開始時のスナップショットを保存。
* **Active:** メモリ上の値をリアルタイム更新（ビューポート即時反映）。
* **Deactivated:** `IsItemDeactivatedAfterEdit()` で確定時にコマンドを発行・履歴へPush。

---

## 8. シリアライズ (nlohmann/json)
* **GUIDベース参照:** JSON出力時、RelationshipのIDは必ず `GUID` に置換して保存。
* **非侵入型実装:** `codegen.py` で `to_json` / `from_json` 関数を自動生成。
* **プレハブ設計:** Entityごとの独立したJSONをシーンファイルからGUIDで参照。

---

## 9. Codegen & スクリプト連携
* **SSOT (schema.yaml):** C++構造体、C#構造体、ImGui描画コードを全自動生成。
* **C#リフレクション:** `[SerializeField]` 属性が付与されたC#変数をCoreCLR経由で抽出し、動的にInspector UIを構成する。