# Editor Improvements Plan (May 2026)

このドキュメントは、`feature/editor_improvements` ブランチで行われるエディタ機能拡張の設計と実装手順をまとめたものです。

## 1. マテリアルアセットの保存機能 (Material Persistence) [DONE]

### 目的
Inspector で編集したマテリアル (`.mat`) のパラメータ（現状は baseColor 等）を即座にソースファイルに書き戻し、変更を永続化する。

### 実装詳細
- **対象ファイル**: `Project/Editor/InspectorView.cpp`
- **処理フロー**:
    1. `.mat` ファイルの描画ループ内で、各 `ImGui::ColorEdit4` などの戻り値を監視。
    2. 変更があった場合、メモリ上の `nlohmann::json` オブジェクトを更新。
    3. ループ終了後、`std::ofstream` を使用してファイルを上書き保存。
- **注意点**: 
    - 現在はアセット単体の編集のみを対象とし、`CommandHistory` (Undo/Redo) への統合は次フェーズとする（アセットファイルの直接編集は ECS のコンポーネント変更とはライフサイクルが異なるため）。

## 2. Scene View へのドラッグ＆ドロップ機能 (D&D Entity Creation) [DONE]

### 目的
Project Browser からアセットを Scene View にドロップすることで、直感的にエンティティを生成できるようにする。

### 実装詳細
- **対象ファイル**: `Project/Editor/SceneView.cpp`
- **処理フロー**:
    1. `SceneView::Render` 内の画像描画後に `ImGui::BeginDragDropTarget()` を追加。
    2. `DND_ASSET_PATH` ペイロードを受け取る。
    3. 拡張子に応じて処理を分岐：
        - `.prefab`: プレハブからエンティティをインスタンス化。
        - `.obj` / `.fbx`: 新規エンティティを作成し、`Transform` と `MeshRenderer` を追加してモデルを割り当て。
    4. 生成位置は、まずはワールド原点（(0,0,0)）またはカメラの前方一定距離とする。
- **検証手順**:
    - `Assets/Prefabs/` 内の `.prefab` を Scene View にドロップし、Hierarchy に追加されることを確認。
    - 生成されたエンティティが正しいモデルを参照していることを確認。

## 3. 検証プロセス [DONE]
1. **ビルド確認**: `premake5` 実行後、`msbuild` 等で Editor プロジェクトのビルドが通ることを確認。 (2026/05/20)
2. **動作確認**:
    - マテリアルを変更し、エディタ再起動後も変更が保持されているか。
    - ドラッグ＆ドロップでエンティティが生成され、Undo で消えるか（可能であれば）。
