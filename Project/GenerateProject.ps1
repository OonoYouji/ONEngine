# # プロジェクトのルートをスクリプトの位置に固定
# $PSScriptRoot | Set-Location

# # premake5.exe が同じフォルダにある前提
# Write-Host "Running Premake..."
# .\premake5.exe vs2022

# if ($LASTEXITCODE -eq 0) {
#     Write-Host "Premake finished. Opening ONEngine.sln..."
#     Start-Process "ONEngine.sln"
# } else {
#     Write-Host "Premake failed with exit code $LASTEXITCODE"
# }

# スクリプトのあるフォルダをプロジェクトルートに固定
$PSScriptRoot | Set-Location

# =========================
# DirectXTex 公式プロジェクトをビルドして .inc ファイル生成
# =========================
$dxtexProj = Join-Path $PSScriptRoot "Externals\DirectXTex\DirectXTex_Desktop_2022.vcxproj"

if (Test-Path $dxtexProj) {
    Write-Host "Building DirectXTex_Desktop_2022.vcxproj to generate compiled files..."

    # MSBuild のパスを探す（Visual Studio がインストールされている前提）
    $msbuildPaths = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
    )

    $msbuildExe = $msbuildPaths | Where-Object { Test-Path $_ } | Select-Object -First 1

    if (-not $msbuildExe) {
        Write-Error "MSBuild.exe not found. Please install Visual Studio with C++ workload."
        exit 1
    }

    # Debug 構成でビルド
    & $msbuildExe $dxtexProj /p:Configuration=Debug /m

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to build DirectXTex_Desktop_2022.vcxproj"
        exit $LASTEXITCODE
    }
    Write-Host "DirectXTex compiled files generated successfully."
} else {
    Write-Warning "DirectXTex_Desktop_2022.vcxproj not found. Skipping shader compilation."
}

# =========================
# Premake 実行
# =========================
Write-Host "Running Premake..."
.\premake5.exe vs2022

if ($LASTEXITCODE -eq 0) {
    Write-Host "Premake finished successfully. Opening ONEngine.sln..."
    Start-Process "ONEngine.sln"
} else {
    Write-Host "Premake failed with exit code $LASTEXITCODE"
}
スクリプトのあるフォルダをプロジェクトルートに固定

