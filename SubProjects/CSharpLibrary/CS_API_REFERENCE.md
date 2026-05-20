# C# スクリプティング API リファレンス

本エンジンにおける C# (Mono) を使用した Runtime 実装の現状の機能をまとめます。

## 1. 基本クラス: `MonoScript`
すべてのユーザースクリプトの基底クラスです。

| メソッド | 説明 |
| :--- | :--- |
| `Awake()` | スクリプト生成時に一度だけ呼ばれます。 |
| `Initialize()` | 全オブジェクトの生成後、最初に一度だけ呼ばれます。 |
| `Update()` | 毎フレーム呼ばれます。 |
| `OnCollisionEnter(Entity collision)` | 衝突開始時に呼ばれます（実装中）。 |

## 2. エンティティとコンポーネント (`Entity`, `Component`)
エンジン内のオブジェクト（Entity）を操作するための基本機能です。

### Entity の操作
- `entity.name`: エンティティ名の取得・設定
- `entity.parent`: 親エンティティの取得・設定
- `entity.enable`: エンティティの有効/無効
- `entity.transform`: Transform コンポーネントへのショートカットアクセス
- `entity.AddComponent<T>()`: 新しいコンポーネントの追加
- `entity.GetComponent<T>()`: 指定したコンポーネントの取得
- `entity.AddScript<T>()`: C# スクリプトの追加
- `entity.GetScript<T>()`: C# スクリプトの取得
- `entity.Destroy()`: エンティティの削除
- `entity.GetOrAddComponent<T>()` [New]: コンポーネントを取得、なければ追加
- `entity.HasComponent<T>()` [New]: コンポーネントの所持確認
- `entity.FindChildRecursive(name)` [New]: 子要素を再帰的に検索
- `entity.DestroyAllChildren()` [New]: 全ての子要素を削除

### ECSGroup
- `ecsGroup.FindEntity(string name)`: 名前でエンティティを検索
- `ecsGroup.CreateEntity(string prefabName)`: プレハブからエンティティを生成

### ObjectUtils [New]
- `ObjectUtils.Instantiate(prefabName, [groupName])`: プレハブから生成
- `ObjectUtils.Instantiate(prefabName, position, rotation, [groupName])`: 位置・回転を指定して生成
- `ObjectUtils.Find(name, [groupName])`: 名前で検索

## 3. Transform コンポーネント
位置、回転、スケールを制御します。

- `transform.position`: `Vector3` (World)
- `transform.rotate`: `Quaternion` (World)
- `transform.scale`: `Vector3` (World)
- `transform.GetDistanceTo(target)` [New]: 対象との距離を計算
- `transform.SetPositionX/Y/Z(val)` [New]: 特定の軸のみ座標を変更
- `transform.SetScaleX/Y/Z(val)` [New]: 特定の軸のみスケールを変更

## 4. 入力システム (`Input`)
キーボード、マウス、ゲームパッドの状態を取得します。

- `Input.GetKey(KeyCode)`: キーが押されているか
- `Input.TriggerKey(KeyCode)`: キーが押された瞬間か
- `Input.KeyboardAxis(KeyboardAxis)`: WASD 等の軸入力
- `Input.GamepadThumb(GamepadAxis)`: スティックの入力 (`Vector2`)
- `Input.TriggerGamepad(Gamepad)`: ボタン入力

## 5. 数学ライブラリ (`Mathf`, `Vector3`, `Quaternion`, `Matrix4x4`)
Unity に近い形式で実装されています。

- **Mathf**: `Sin`, `Cos`, `Atan2`, `Lerp`, `Clamp`, `Deg2Rad`, `Rad2Deg` 等
- **MathUtils** [New]: 
    - `MoveTowards`: 定速移動
    - `SmoothDamp`: 滑らかな追従（減衰あり）
    - `Remap`: 範囲変換
    - `RandomOnUnitSphere`: 半径1の球体上のランダム座標
- **Vector3**:
 `Normalize`, `Dot`, `Cross`, `Length`, `Lerp` 等
- **Quaternion**: `FromEuler`, `LookAt`, `Identity`, `Inverse`, `ToEuler`, `*`演算子(回転の合成)
- **Matrix4x4**: 各種行列演算

## 6. デバッグツール (`Debug`)
ログ出力に使用します。

- `Debug.Log(string)`: 標準ログ
- `Debug.LogInfo(string)`: 情報ログ
- `Debug.LogWarning(string)`: 警告ログ
- `Debug.LogError(string)`: エラーログ

---
## 実装例
```csharp
public class MyScript : MonoScript {
    public override void Update() {
        if (Input.TriggerKey(KeyCode.Space)) {
            Debug.Log("Jumped!");
            Vector3 pos = transform.position;
            pos.y += 1.0f;
            transform.position = pos;
        }
    }
}
```
