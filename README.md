# ビルド状態
[![DebugBuild](https://github.com/OonoYouji/ONEngine/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/OonoYouji/ONEngine/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/OonoYouji/ONEngine/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/OonoYouji/ONEngine/actions/workflows/ReleaseBuild.yml)



# ビルド手順
- "GenerateProject.ps1"をPowerShellで起動
- 生成された"ONEngine.sln"を起動、visual studio 2026でビルド


# 注意点
- このエンジンを配置するパスの中に日本語が混じっているとC#スクリプトが起動できずクラッシュします
そのため配置するフォルダへのパスはすべて英語になるように注意してください  

- このエンジンではMeshShaderを使用しているため対応していないGPUでは起動できません  

| メーカー       | 世代 / シリーズ                                | Mesh Shader 対応  |
| ---------- | ---------------------------------------- | --------------- |
| **NVIDIA** | GeForce 40シリーズ (Ada)                     | ✅ 対応            |
|            | GeForce 30シリーズ (Ampere)                  | ✅ 対応            |
|            | GeForce 20シリーズ (Turing)                  | ✅ 対応            |
|            | GTX 16シリーズ (Turing, GPUにより制限あり)          | ⚠ 一部制限あり（基本非対応） |
|            | GTX 10シリーズ以前 (Pascal / Maxwell / Kepler) | ❌ 非対応           |
| **AMD**    | RX 7000シリーズ (RDNA 3)                     | ✅ 対応            |
|            | RX 6000シリーズ (RDNA 2)                     | ✅ 対応            |
|            | RX 5000シリーズ以前 (RDNA 1 / GCN)             | ❌ 非対応           |
| **Intel**  | Xe (DG1 / DG2)                           | ✅ 対応            |
|            | 第10世代以前の統合GPU / Iris Plus 系              | ❌ 非対応           |
