@echo off
setlocal enabledelayedexpansion

REM DLLの出力フォルダ
set OUTPUT_DIR=..\..\Packages\Scripts

REM 元のDLL名
set ORIGINAL_DLL=CSharpLibrary.dll

REM 現在日時を取得してファイル名用に整形（yyyyMMdd_HHmmss）
for /f "tokens=1-4 delims=/ " %%a in ('date /t') do (
    set DATE=%%c%%a%%b
)
for /f "tokens=1-2 delims=:." %%a in ('time /t') do (
    set TIME=%%a%%b
)
set TIME=%TIME: =0%
set DATETIME=%DATE%_%TIME%

REM 新しいDLL名
set RENAMED_DLL=CSharpLibrary_%DATETIME%.dll

echo Renaming %OUTPUT_DIR%\%ORIGINAL_DLL% to %OUTPUT_DIR%\%RENAMED_DLL%
move /Y "%OUTPUT_DIR%\%ORIGINAL_DLL%" "%OUTPUT_DIR%\%RENAMED_DLL%"

endlocal
