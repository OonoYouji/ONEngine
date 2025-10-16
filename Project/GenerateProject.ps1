# プロジェクトのルートをスクリプトの位置に固定
$PSScriptRoot | Set-Location

# premake5.exe が同じフォルダにある前提
Write-Host "Running Premake..."
.\premake5.exe vs2022

if ($LASTEXITCODE -eq 0) {
    Write-Host "Premake finished. Opening ONEngine.sln..."
    Start-Process "ONEngine.sln"
} else {
    Write-Host "Premake failed with exit code $LASTEXITCODE"
}