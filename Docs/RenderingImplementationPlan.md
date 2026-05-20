# Rendering Implementation Plan - ONEngine (feature/new_engine)

このドキュメントでは、ONEngine のレンダリング機能強化に向けた実装計画を定義します。

## 1. 優先目標: ポストプロセス (Bloom) の完全実装
現在、`PostProcessSystem` は枠組みのみが存在し、実際の Bloom 処理がダミーとなっています。これを完全に動作させ、絵作りのクオリティを向上させます。

### 実装フェーズ
1.  **高輝度抽出 (Threshold Pass)**
    *   `BloomThreshold.hlsl` を使用。
    *   メインテクスチャから指定した閾値以上のピクセルを抽出。
2.  **ガウシアン / デュアルフィルタリング (Downsample/Upsample)**
    *   `PostProcessSystem` で確保済みの `downsampleBuffers_` / `upsampleBuffers_` を使用。
    *   各レベルで `Blur.hlsl` を適用しながら縮小・拡大を行う。
3.  **最終合成 (Final Composition)**
    *   `PostProcess.hlsl` にて、メインテクスチャに Bloom テクスチャを加算合成。
    *   既存の ACES トーンマッピングと組み合わせて適切な HDR -> SDR 変換を維持。

## 2. 中期目標: シャドウマッピング (Shadow Mapping)
旧エンジンには存在した影の実装が新エンジンでは未実装のため、これを現代的な手法で再構築します。

### 実装予定内容
*   **Depth Pass (Shadow Map Generation)**
    *   ライト視点からの深度バッファ描画。
*   **Cascaded Shadow Maps (CSM)**
    *   広大なシーンに対応するため、視錐台を分割したシャドウマップの適用。
*   **PCF (Percentage Closer Filtering)**
    *   影の縁を滑らかにするためのフィルタリング。

## 3. 最適化: Z-Prepass
現在空実装となっている `Renderer::RenderZPrepass` を実装します。

### 実装内容
*   カラー出力を無効化した深度のみのパスを先に行い、メインパスでのオーバードローを削減します。

## 4. 特殊エフェクト: アウトラインの洗練
既存の `PostProcess.hlsl` にある ID/Normal ベースのアウトライン機能をより使いやすく調整します。

---

## 技術スタック
*   **API:** DirectX 12
*   **Shader:** HLSL (Shader Model 6.x)
*   **Techniques:** GPU Culling, Clustered Lighting, Compute Shaders (for Blur)
