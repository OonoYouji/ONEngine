# Hybrid ECS Engine: レンダリングパイプライン詳細設計書 (v5.1)

## 1. 概要
本ドキュメントは、C++による高速なレンダリングコアとC#によるスクリプト層を統合したハイブリッドECSエンジンにおける、描画オブジェクトの定義およびレンダリングパイプラインの実行順序を規定するものである。
特に、フォトリアルとアニメ調の混在（背景はリアル、キャラはアニメ等）を可能にする **Forward+ ＋ MRT (G-Buffer)** のハイブリッド構成を中核とする。

## 2. 描画対象オブジェクト一覧
本エンジンで描画をサポートする主なオブジェクト種別は以下の通り。

| オブジェクト種別 | 説明 | 技術的特徴 |
| :--- | :--- | :--- |
| **Static Mesh** | 背景、建物、プロップ等の静的オブジェクト | Geometry Poolによる一括管理、GPU-Driven Culling対象 |
| **Skinned Mesh** | キャラクター、敵等の動的エンティティ | 最大4ボーンの影響を計算する頂点スキニング ($V_{skinned}$)、Geometry Pool管理 |
| **Particle** | 弾、エフェクト、魔法等の半透明体 | アルファブレンド/加算合成、GPUソート、Clustered Lighting適用 |
| **UI / Debug** | HUD、スコア、デバッグ用ギズモ等 | 独立したViewContextによる最前面オーバーレイ描画 |

## 3. レンダリングパイプライン詳細 (Hybrid Forward+ / MRT)
本エンジンは、Forward 描画の柔軟性と MRT (Multi-Render Target) によるポストプロセスの強力な制御を両立させる。

### G-Buffer 構成
Main Pass (不透明描画) では、以下の 3 枚のターゲットへ同時に出力する。
*   **RT0 (Color):** `R16G16B16A16_FLOAT` (HDR カラー)
*   **RT1 (Normal):** `R16G16B16A16_FLOAT` (ワールド空間法線)
*   **RT2 (ID/Flags):** `R32G32_UINT` (EntityID & ポストプロセス制御フラグ)

### レンダリング順序

### Step 1: Extract Phase (データ抽出)
* C#側の `Update()` 完了後、ECSから描画に必要なデータをレンダリング用の `StructuredBuffer` へコピーする。
* フレームリソースの多重化（Double/Triple Buffering）により、CPUによるコマンド構築とGPUの実行を非同期並列化する。

### Step 2: Frustum Culling (フラスタムカリング)
* **対象:** Static Mesh, Skinned Mesh の全インスタンス
* **内容:** Compute Shaderを用いて、カメラの視錐台に基づくカリングを実行する。

### Step 3: Z-Prepass (深度先行描画)
* 深度情報のみをZバッファに書き込み、後続処理の基盤となるDepthを作成する。

### Step 4: HZB Occlusion Culling (オクルージョンカリング)
* Z-Prepass 結果から HZB (Hierarchical Z-Buffer) を構築し、遮蔽カリングを実行。

### Step 5: Shadow Pass (シャドウマップ生成)
* ディレクショナルライト等からの視点で深度を描画し、CSM（Cascaded Shadow Maps）を生成。

### Step 6: Light Culling (クラスタライトカリング)
* Zバッファに基づき画面を 3D クラスタに分割。各ピクセルに影響する動的ライトリストを Compute Shader で生成する。

### Step 7: Main Pass (不透明・ハイブリッド描画)
* **内容:** 
    * 前述の **G-Buffer (RT0-2)** に対して出力。
    * **フォトリアル背景:** PBR シェーダーを使用し、リアルな反射とライティングを計算。
    * **アニメ調キャラ:** セルシェーダーを使用し、ライトの階調化やアニメ用特殊テクスチャを適用。
    * `RT2` に「アニメ調対象」などのフラグを書き込むことで、後続のポストプロセスで個別の処理（アウトライン等）を可能にする。

### Step 8: Transparent Sort & Pass (半透明描画)
* 距離とインデックスのペアを GPU 上でソートし、奥から順に描画。Clustered Lighting を適用。

### Step 9: Post-Processing (インテリジェント・ポストプロセス)
* **Bloom:** 高輝度部の漏れ出し。
* **Outline Extraction:** `RT1 (Normal)` と `RT2 (ID)` を参照し、物体境界や法線不連続点を検出。
* **Selective Outline:** `RT2` のフラグに基づき、キャラのみにアウトラインを適用する、といった選択的処理を行う。
* **Tone Mapping & Gamma:** HDR カラーを SDR (sRGB) に変換。

### Step 10: Multi-View & UI Pass (最終出力)
* ViewContext に基づき、UI 等を最前面に描画。


---
*Technical Design Supplement v5.1 - 2026/05*