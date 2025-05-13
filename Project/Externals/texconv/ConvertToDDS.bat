@echo off
setlocal

:: texconv.exe の絶対パス（必要に応じて修正）
set TEXCONV_PATH=C:\Users\k023g\source\repos\2023_CG2\ONEngine\Project\Externals\texconv\texconv.exe

:: バッチファイルのあるディレクトリをベースにパスを定義
set BASEDIR=%~dp0
set INPUT_DIR=%BASEDIR%srcTex
set OUTPUT_DIR=%BASEDIR%output

:: 出力フォルダがなければ作成
if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

echo === Starting Texture Conversion ===

:: PNGファイルの変換 (フォーマット: DXT5)
for %%F in ("%INPUT_DIR%\*.png") do (
    echo Converting PNG: %%~nxF
    "%TEXCONV_PATH%" -f DXT5 -m 0 -o "%OUTPUT_DIR%" "%%F"
)

:: HDRファイルの変換 (フォーマット: R32G32B32A32_FLOAT)
for %%F in ("%INPUT_DIR%\*.hdr") do (
    echo Converting HDR: %%~nxF
    "%TEXCONV_PATH%" -f R32G32B32A32_FLOAT -m 1 -o "%OUTPUT_DIR%" "%%F"
)

echo === All Conversions Complete ===
pause
