# Hybrid ECS Component Data Design (Unity to ECS)

このドキュメントは、Unityの主要なコンポーネントを純粋なデータ（POD）としてECS向けに再設計したデータ定義です。

## 1. 基本 (General)

Unityの `Transform` と `RectTransform` の機能を、役割ごとに分割します。

### 1.1. Transform 系
*   **`LocalTransform` (Input / State)**
    *   役割: ユーザー（C#）が操作するローカル座標。
    *   データ: `position` (vec3), `rotation` (vec4/quat), `scale` (vec3)
*   **`WorldMatrix` (Output / GPU Buffer)**
    *   役割: C++システムが計算し、GPUや物理エンジンに渡す絶対座標行列。
    *   データ: `matrix` (mat4)
*   **`Parent` (Hierarchy)**
    *   役割: 階層構造の構築。
    *   データ: `parentId` (uint)

### 1.2. RectTransform 系 (UI用)
*   **`LocalRect` (Input)**
    *   役割: UIのアンカー、ピボット、ローカル配置データ。
    *   データ: `anchorMin` (vec2), `anchorMax` (vec2), `pivot` (vec2), `anchoredPosition` (vec2), `sizeDelta` (vec2)
*   **`CanvasWorldMatrix` (Output / GPU Buffer)**
    *   役割: Canvasの解像度計算を加味した最終的なUI描画用行列。
    *   データ: `matrix` (mat4)

---

## 2. 物理演算 (Physics)

キャッシュヒット率を最大化するため、毎フレーム更新される「状態」と、滅多に変わらない「パラメータ」に分割します。

### 2.1. Rigidbody
*   **`Velocity` (State)**
    *   役割: 毎フレームの物理更新・移動処理に使われる動的データ。
    *   データ: `linear` (vec3), `angular` (vec3)
*   **`PhysicsProperties` (Parameter)**
    *   役割: 質量や重力設定などの静的パラメータ。
    *   データ: `inverseMass` (float), `inverseInertia` (vec3), `useGravity` (bool), `isKinematic` (bool), `padding` (vec2)

### 2.2. Collider
*   **`BoxGeometry` / `SphereGeometry` / `CapsuleGeometry` (Parameter)**
    *   役割: 衝突判定の形状データ。
    *   データ (Box): `center` (vec3), `extents` (vec3)
    *   データ (Sphere): `center` (vec3), `radius` (float)
    *   データ (Capsule): `center` (vec3), `radius` (float), `height` (float), `direction` (int)
*   **`ColliderFilter` (Parameter)**
    *   役割: 衝突レイヤーとマスク設定。
    *   データ: `layer` (uint), `collisionMask` (uint), `isTrigger` (bool)

---

## 3. 表示・レンダリング (Rendering)

実体へのポインタは持たず、すべてアセット管理システムの「リソースID」として定義し、GPU転送を最適化します。

*   **`CameraData` (Parameter)**
    *   役割: カメラの投影設定。
    *   データ: `fov` (float), `nearClip` (float), `farClip` (float), `cullingMask` (uint)
*   **`LightData` (Parameter / GPU Buffer)**
    *   役割: 光源設定。
    *   データ: `type` (int), `color` (vec3), `intensity` (float), `range` (float), `spotAngle` (float)
*   **`RenderMesh` (Parameter / GPU Buffer)**
    *   役割: 描画するメッシュとマテリアルのハンドル。
    *   データ: `meshId` (uint), `materialId` (uint), `renderLayer` (uint), `isVisible` (bool)
*   **`SpriteRenderData` (Parameter / GPU Buffer)**
    *   役割: 2Dスプライトの描画情報。
    *   データ: `spriteId` (uint), `materialId` (uint), `color` (vec4), `flipX` (bool), `flipY` (bool)

---

## 4. オーディオ (Audio)

再生状況（State）と音源設定（Parameter）を分割します。

*   **`AudioSourceData` (Parameter)**
    *   役割: 音源の設定パラメータ。
    *   データ: `clipId` (uint), `volume` (float), `pitch` (float), `spatialBlend` (float), `loop` (bool)
*   **`AudioPlayState` (State)**
    *   役割: 現在の再生状況。C#から再生コマンドを送るのにも使用。
    *   データ: `isPlaying` (bool), `time` (float), `triggerPlay` (bool)
*   **`AudioListenerTag` (Tag)**
    *   役割: マイクとなるエンティティを示すタグ（データなし）。

---

## 5. UI (User Interface)

文字列（String）は参照型でBlittableではないため、テキストは「StringHash（ID）」で管理するアプローチをとります。

*   **`CanvasData` (Parameter)**
    *   役割: UIのルート設定。
    *   データ: `renderMode` (int), `scaleFactor` (float)
*   **`UIImageData` (Parameter / GPU Buffer)**
    *   役割: UI画像の描画情報。
    *   データ: `spriteId` (uint), `color` (vec4), `materialId` (uint)
*   **`UITextData` (Parameter / GPU Buffer)**
    *   役割: テキストの描画情報。可変長文字列の代わりにHash/IDを使用。
    *   データ: `fontId` (uint), `textHashId` (uint), `fontSize` (float), `color` (vec4)
*   **`UIButtonState` (State)**
    *   役割: ボタンのインタラクション状態。C#のシステムが読み取ってイベントを発火する。
    *   データ: `isHovered` (bool), `isPressed` (bool), `interactable` (bool)

---

## 6. アニメーション (Animation)

*   **`AnimatorState` (State)**
    *   役割: アニメーションコントローラーの現在状態。
    *   データ: `controllerId` (uint), `currentClipHash` (uint), `normalizedTime` (float), `speed` (float)

---

## 7. ナビゲーション (Navigation)

*   **`NavAgentParams` (Parameter)**
    *   役割: エージェントの静的パラメータ。
    *   データ: `radius` (float), `height` (float), `maxSpeed` (float), `avoidancePriority` (int)
*   **`NavAgentState` (State)**
    *   役割: 移動の目標と現在の状況。
    *   データ: `destination` (vec3), `currentVelocity` (vec3), `hasPath` (bool), `remainingDistance` (float)

---

## 8. スクリプト・システム制御

*   **`ScriptComponent` (System)**
    *   役割: C#オブジェクト（MonoBehaviour相当）のGCハンドル。
    *   データ: `typeId` (int), `gcHandle` (uint64)
*   **`SceneMask` (System)**
    *   役割: マルチシーン実行時のフィルタリング用ビットマスク。
    *   データ: `mask` (uint)