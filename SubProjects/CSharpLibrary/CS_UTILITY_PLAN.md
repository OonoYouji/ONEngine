# C# ユーティリティ関数 実装計画書

ユーザーがゲーム開発を効率的に行えるよう、便利なユーティリティクラスおよび拡張メソッドを整備します。

## 1. カテゴリ別機能詳細

### A. エンティティ操作 (`EntityExtensions`)
ECSベースの操作を簡略化し、Unityライクな直感的な操作を提供します。
- `GetOrAddComponent<T>`: コンポーネントの存在確認と追加を一度に行う。
- `HasComponent<T>`: コンポーネントの所持確認。
- `FindChildRecursive`: 子要素を再帰的に検索。
- `DestroyAllChildren`: 子要素を一括削除。

### B. 数学・補間 (`MathUtils` / `VectorExtensions`)
ゲームロジックで多用される計算を共通化します。
- `MoveTowards`: 定速移動。
- `SmoothDamp`: 滑らかな追従（減衰あり）。
- `Remap`: 数値範囲の再マッピング。
- `RandomOnUnitSphere`: ランダムな方向ベクトルの生成。

### C. トランスフォーム制御 (`TransformExtensions`)
Transformコンポーネントの操作を拡張します。
- `LookAtSmooth`: 指定方向への滑らかな回転。 (Quaternion.Slerp等で実現可能なため、今回はSet系を優先)
- `GetDistanceTo`: 対象との距離計算。
- `SetX / SetY / SetZ`: 特定の軸のみの値を変更。

### D. シーン・オブジェクト管理 (`ObjectUtils`)
ライフサイクル管理を補助します。
- `Instantiate`: 指定座標・回転でのプレハブ生成。
- `FindWithTag`: 特定のタグ（または名前ルール）を持つエンティティの検索。

---

## 2. 実装タスクリスト

### フェーズ 1: コア・ユーティリティ (優先度: 高)
- [x] `EntityExtensions.cs` の作成
    - [x] `GetOrAddComponent<T>` の実装
    - [x] `HasComponent<T>` の実装
- [x] `MathUtils.cs` の作成
    - [x] `MoveTowards` (float, Vector3) の実装
    - [x] `Remap` の実装

### フェーズ 2: 応用操作 (優先度: 中)
- [x] `TransformExtensions.cs` の作成
    - [x] `GetDistanceTo` の実装
    - [x] `SetPositionX/Y/Z` 等のアクセサ実装
- [x] `EntityExtensions.cs` の拡張
    - [x] `FindChildRecursive` の実装
    - [x] `DestroyAllChildren` の実装

### フェーズ 3: 高度な機能 (優先度: 低)
- [x] `ObjectUtils.cs` の作成
    - [x] `Instantiate` ショートカットの実装
    - [x] `Find` の実装
- [x] `MathUtils.cs` の拡張
    - [x] `SmoothDamp` の実装
    - [x] `RandomOnUnitSphere` の実装

---

## 3. 実装の進め方
1. [x] 各フェーズごとに新規ファイルを作成、または既存クラスを拡張。
2. [x] `CSharpLibrary.csproj` にファイルを追加。
3. [x] ビルド確認を行い、正常に動作することを確認。
4. [x] `CS_API_REFERENCE.md` に追加した機能を追記。
