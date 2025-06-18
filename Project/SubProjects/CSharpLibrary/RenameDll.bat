@echo off
setlocal enabledelayedexpansion

REM DLLの出力フォルダ（適宜変更）
set OUTPUT_DIR=..\..\Packages\Scripts

REM 元のDLL名
set ORIGINAL_DLL=CSharpLibrary.dll

REM 日付と時刻を取得してフォーマット yyyyMMdd_HHmmss
for /f "tokens=1-3 delims=/- " %%a in ('date /t') do (
  set MM=%%a
  set DD=%%b
  set YY=%%c
)

for /f "tokens=1-2 delims=:." %%a in ('time /t') do (
  set HH=%%a
  set Min=%%b
)

REM 秒を取得
for /f "tokens=1 delims=." %%a in ("%TIME%") do set Sec=%%a
set Sec=%Sec:~6,2%

REM 先頭に0がついていない場合の補正（例：8→08）
if 1%HH% LSS 110 set HH=0%HH%
if 1%Min% LSS 110 set Min=0%Min%
if 1%Sec% LSS 110 set Sec=0%Sec%

REM Windowsの日付フォーマットが異なるケースがあるので注意
REM yyyyMMdd_HHmmss 形式でセット
set TIMESTAMP=%YY%%MM%%DD%_%HH%%Min%%Sec%

REM 新しいDLL名
set RENAMED_DLL=CSharpLibrary_%TIMESTAMP%.dll

echo Renaming %OUTPUT_DIR%\%ORIGINAL_DLL% to %OUTPUT_DIR%\%RENAMED_DLL%
move /Y "%OUTPUT_DIR%\%ORIGINAL_DLL%" "%OUTPUT_DIR%\%RENAMED_DLL%"

endlocal
