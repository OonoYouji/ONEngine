# プロジェクトのルートをスクリプトの位置に固定
$PSScriptRoot | Set-Location

# =========================
# DirectXTex 公式プロジェクトをビルドして .inc ファイル生成
# =========================
$dxtexProj = Join-Path $PSScriptRoot "Externals\DirectXTex\DirectXTex_Desktop_2022.vcxproj"

if ($LASTEXITCODE -eq 0) {
    Write-Host "Premake finished. Opening ONEngine.sln..."
    Start-Process "ONEngine.sln"
} else {
    Write-Host "Premake failed with exit code $LASTEXITCODE"
}
