# Editor Functionality Verification Guide

エディタの全機能が正常に動作するかを確認するためのチェックリストです。
ビルド済みのエディタを起動し、以下の手順を実施してください。

## 準備
1. エディタを起動します。
2. 上部メニュー `File -> Load Scene` から `Assets/Scene/EditorTestScene.scene` を選択して読み込みます。

## 1. ギズモと選択 (Scene View / Hierarchy)
- [x] **マウスピッキング**: Scene View 上の "Test Cube" をクリックし、Hierarchy と Inspector で選択状態になることを確認。
- [ ] **ギズモ操作**: W/E/R キーでギズモを切り替え、オブジェクトの移動・回転・スケールが変化することを確認。
- [x] **Undo/Redo**: Transform を変更した後、`Ctrl+Z` で元に戻り、`Ctrl+Y` でやり直せることを確認。

## 2. マテリアル保存機能 (Inspector) [NEW]
- [x] **アセット選択**: Project Browser で `Assets/Material/EditorTest.mat` をクリックして選択。
- [x] **パラメータ編集**: Inspector に表示された `baseColor` を変更。
- [x] **永続化確認**: エディタを一度終了し、再度 `Assets/Material/EditorTest.mat` を開いて色が維持されていることを確認（またはファイルをテキストエディタで開き、JSONが更新されているか確認）。

## 3. ドラッグ＆ドロップ生成 (Project Browser -> Scene View) [NEW]
- [ ] **プレハブ生成**: Project Browser から `Assets/Prefabs/EditorTestObject.prefab` を Scene View へドロップ。Hierarchy に新しいエンティティが生成されることを確認。
- [ ] **モデル生成**: `Assets/Models/` 内の `.obj` ファイルを Scene View へドロップ。新規エンティティが作成され、MeshRenderer にモデルが割り当てられることを確認。

## 4. 階層管理 (Hierarchy)
- [ ] **親子化**: Hierarchy 上で "Test Cube" を "Main Camera" にドラッグ＆ドロップし、親子関係が構築されることを確認。
- [ ] **並べ替え**: エンティティをドラッグして上下の順序を入れ替えられることを確認。

## 5. アセット管理 (Project Browser)
- [ ] **新規作成**: 右クリックメニュー `Create -> Material` で新しいマテリアルが作成できることを確認。
- [ ] **削除・複製**: `Assets/Material/EditorTest.mat` を右クリックして `Duplicate` し、その後削除できることを確認。
