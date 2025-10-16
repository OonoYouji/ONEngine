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


# プロジェクトのルートをスクリプトの位置に固定
$PSScriptRoot | Set-Location

# premake5.exe が同じフォルダにある前提
Write-Host "Running Premake..."
.\premake5.exe vs2022

if ($LASTEXITCODE -eq 0) {
    Write-Host "Premake finished successfully."
    
    # ==========================================
    # DirectXTex シェーダーの .inc ファイル生成
    # ==========================================
    $dxtexPath = Join-Path $PSScriptRoot "Externals\DirectXTex"
    $compilePath = Join-Path $dxtexPath "Shaders\Compile"

    if (-not (Test-Path $dxtexPath)) {
        Write-Host "DirectXTex not found at: $dxtexPath"
    }
    else {
        Write-Host "Generating DirectXTex shader includes..."
        Set-Location $dxtexPath

        if (-not (Test-Path $compilePath)) {
            New-Item -ItemType Directory -Path $compilePath | Out-Null
        }

        $scripts = @(
            "MakeBC4EncodeCS.bat",
            "MakeBC5EncodeCS.bat",
            "MakeBC6HEncodeCS.bat",
            "MakeBC7EncodeCS.bat"
        )

        foreach ($script in $scripts) {
            if (Test-Path $script) {
                Write-Host "Running $script..."
                cmd /c $script
            } else {
                Write-Host "⚠️  $script not found, skipping."
            }
        }

        Write-Host "DirectXTex shader include generation complete."
        Set-Location $PSScriptRoot
    }

    # ==========================================
    # ソリューションを開く
    # ==========================================
    Write-Host "Opening ONEngine.sln..."
    Start-Process "ONEngine.sln"
} 
else {
    Write-Host "Premake failed with exit code $LASTEXITCODE"
}
スクリプトのあるフォルダをプロジェクトルートに固定

