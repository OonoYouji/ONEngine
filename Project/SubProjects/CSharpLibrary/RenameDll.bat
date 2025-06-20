@echo off
setlocal enabledelayedexpansion

REM DLLの出力フォルダ（適宜変更）
set OUTPUT_DIR=..\..\Packages\Scripts

REM 元のDLL名
set ORIGINAL_DLL=CSharpLibrary.dll

REM 日付と時刻を取得してフォーマット yyyyMMdd_HHmmss
for /f "tokens=1-3 delims=/.- " %%a in ('date /t') do (
  set YY=%%a
  set MM=%%b
  set DD=%%c
)

REM 遅延展開で年を修正
set YY=!YY!
if not "!YY:~0,2!"=="20" (
  set YY=20!YY!
)

REM 時刻の取得（ゼロ埋め済み）
for /f %%a in ('powershell -Command "Get-Date -Format \"HHmmss\""') do set TIMESTR=%%a

REM yyyyMMdd_HHmmss 形式に組み立て
set TIMESTAMP=!YY!!MM!!DD!_!TIMESTR!

REM 新しいDLL名
set RENAMED_DLL=CSharpLibrary_!TIMESTAMP!.dll

echo Renaming %OUTPUT_DIR%\%ORIGINAL_DLL% to %OUTPUT_DIR%\%RENAMED_DLL%
move /Y "%OUTPUT_DIR%\%ORIGINAL_DLL%" "%OUTPUT_DIR%\%RENAMED_DLL%"

endlocal
