# ビルド状態
[![DebugBuild](https://github.com/OonoYouji/ONEngine/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/OonoYouji/ONEngine/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/OonoYouji/ONEngine/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/OonoYouji/ONEngine/actions/workflows/ReleaseBuild.yml)

# ビルド手順
- `GenerateProject.ps1` をPowerShellで実行
- 生成された `ONEngine.sln` を起動し、Visual Studio 2026でビルド

# 注意点
- **C#スクリプトの実行環境とパスの制限について**
  本エンジンは、C#スクリプトの実行に **Mono** を使用しています。
  仕様上、実行ファイル（`.exe`）までのパスに日本語などのマルチバイト文字が含まれていると、Monoが正常に動作せずクラッシュしてしまいます。
  そのため、エンジンを配置するフォルダおよび実行ファイルまでのパスは、**すべて半角英数字**になるように注意してください。

- **GPUの動作要件について**
  このエンジンではMeshShaderを使用しているため、対応していないGPUでは起動できません。

| メーカー       | 世代 / シリーズ                                  | Mesh Shader 対応  |
| ---------- | ---------------------------------------- | --------------- |
| **NVIDIA** | GeForce 40シリーズ (Ada)                     | ✅ 対応             |
|            | GeForce 30シリーズ (Ampere)                  | ✅ 対応             |
|            | GeForce 20シリーズ (Turing)                  | ✅ 対応             |
|            | GTX 16シリーズ (Turing, GPUにより制限あり)          | ⚠ 一部制限あり（基本非対応） |
|            | GTX 10シリーズ以前 (Pascal / Maxwell / Kepler) | ❌ 非対応            |
| **AMD** | RX 7000シリーズ (RDNA 3)                     | ✅ 対応             |
|            | RX 6000シリーズ (RDNA 2)                     | ✅ 対応             |
|            | RX 5000シリーズ以前 (RDNA 1 / GCN)             | ❌ 非対応            |
| **Intel** | Xe (DG1 / DG2)                           | ✅ 対応             |
|            | 第10世代以前の統合GPU / Iris Plus 系              | ❌ 非対応            |
