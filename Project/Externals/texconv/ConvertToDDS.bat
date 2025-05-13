@echo off
setlocal

:: texconv.exe のパス
set TEXCONV_PATH=C:\Users\k023g\source\repos\2023_CG2\ONEngine\Project\Externals\texconv\texconv.exe

:: このバッチファイルがあるフォルダを基準に相対パスを解釈
set BASEDIR=%~dp0

:: 入力フォルダと出力フォルダ（絶対パスに展開される）
set INPUT_DIR=%BASEDIR%srcTex
set OUTPUT_DIR=%BASEDIR%output

:: 出力形式
set FORMAT=DXT5

:: 出力フォルダがなければ作成
if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

echo === Starting PNG to DDS conversion ===

:: .pngファイルをループして変換
for %%F in ("%INPUT_DIR%\*.png") do (
    echo Converting: %%~nxF
    "%TEXCONV_PATH%" -f %FORMAT% -m 0 -o "%OUTPUT_DIR%" "%%F"
)

echo === Conversion complete ===
pause
