# Hybrid ECS Engine 開発者ガイド (Unityスタイル API)

本エンジンは、C++による高速な ECS コアと、C# による柔軟なスクリプト層を組み合わせたハイブリッドエンジンです。Unity に近い感覚で開発ができるように設計されています。

## 1. クイックスタート

### ビルドと実行
1.  **C# スクリプトのビルド:**
    ```powershell
    dotnet build source/script_api/EcsApp.csproj -c Debug
    ```
2.  **エンジンの起動:**
    `bin/Debug/Temp.exe` を実行します。

### ホットリロード
エンジンを実行したまま C# のコードを書き換え、再度 `dotnet build` を実行してください。エンジンが自動的に変更を検知し、数秒以内に新しいロジックが適用されます。

---

## 2. スクリプトの書き方 (Unity スタイル)

すべてのスクリプトは `GameScript` を継承します。

```csharp
using System;
using Engine.Core;
using Engine.Generated;

public class MyPlayerScript : GameScript
{
    private float _speed = 2.0f;

    public override void Update()
    {
        // 1. Transform へのアクセス (Unity互換)
        var tr = transform;
        tr.position.x += _speed * 0.01f;
        transform = tr; // 値を書き換えたら再代入して反映

        // 2. 他のコンポーネントの取得 (Unity互換)
        if (GetComponent<Rigidbody>().useGravity) {
            // 重力を使っている場合の処理
        }

        // 3. ImGui によるデバッグ表示
        ImGui.Begin("Player Settings");
        ImGui.SliderFloat("Speed", ref _speed, 0.1f, 10.0f);
        ImGui.End();
    }
}
```

---

## 3. ECS クエリシステム (他エンティティの検索)

特定のコンポーネントを持つエンティティを高速に列挙できます。

```csharp
// Transform を持つすべてのエンティティを走査
foreach (var entityId in World.Query<Transform>())
{
    var targetTr = World.GetComponent<Transform>(entityId);
    // 距離判定などのロジック...
}

// 複数のコンポーネントを両方持つものを検索
foreach (var enemyId in World.Query<Transform, Rigidbody>())
{
    // ...
}
```

---

## 4. データ駆動 (JSON によるエンティティ定義)

エンティティの構成（コンポーネントとスクリプト）は `data/*.json` で定義します。

`data/player.json`:
```json
{
  "name": "PlayerCharacter",
  "components": {
    "Transform": {
      "position": [0, 5, 0],
      "rotation": [0, 0, 0],
      "scale": [1, 1, 1]
    },
    "Rigidbody": {
      "mass": 1.0,
      "useGravity": true
    }
  },
  "scripts": [
    "MyPlayerScript"
  ]
}
```

---

## 5. コンポーネントの追加・変更 (Codegen)

新しいデータ構造が必要な場合は、`tools/schema.yaml` を編集します。

1.  `tools/schema.yaml` に定義を追加：
    ```yaml
    Health:
      fields:
        current: { type: float, default: 100.0 }
        max: { type: float, default: 100.0 }
    ```
2.  コード生成を実行：
    ```powershell
    python tools/codegen.py
    ```
    これにより、C++ 構造体、C# 構造体、および通信用の Interop コードが自動生成されます。

---

## 6. エンジン構造の対応表

| Unity 概念 | 本エンジンでの対応 | 備考 |
| :--- | :--- | :--- |
| GameObject | **Entity (ulong)** | ただの ID です。 |
| MonoBehaviour | **GameScript** | `Update()` 等を記述します。 |
| Transform | **Transform (Component)** | `schema.yaml` で定義された構造体。 |
| Prefab | **JSON ファイル** | `data/` 以下のファイル。 |
| Play ボタン | **Temp.exe 起動** | 実行中に C# をビルドすれば即座に反映。 |

---
*作成日: 2026年5月1日*
