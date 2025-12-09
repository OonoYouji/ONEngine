@echo off
setlocal enabledelayedexpansion

REM --- Determine the output folder relative to the batch file location ---
REM %~dp0 is the folder where this batch file resides (includes trailing \)
set "SCRIPT_DIR=%~dp0"
REM OUTPUT_DIR is relative to the batch location (adjust according to your project structure)
set "OUTPUT_DIR=%SCRIPT_DIR%..\..\Project\Packages\Scripts"

REM Check if the output folder exists
pushd "%OUTPUT_DIR%" >nul 2>&1
if errorlevel 1 (
    echo ERROR: Output folder does not exist: %OUTPUT_DIR%
    popd
    endlocal
    exit /b 2
)
set "OUTPUT_DIR=%CD%"
popd

REM Original DLL and PDB
set "ORIGINAL_DLL=CSharpLibrary.dll"
set "ORIGINAL_PDB=CSharpLibrary.pdb"

REM Get current date (yyyyMMdd) using WMIC to avoid locale dependency
for /f "skip=1 tokens=1 delims=." %%A in ('wmic os get LocalDateTime ^| findstr /r /v "^$"') do set ldt=%%A
REM Example ldt: 20251202091530.000000+090
set "YY=!ldt:~0,4!"
set "MM=!ldt:~4,2!"
set "DD=!ldt:~6,2!"

REM Get current time using PowerShell, fallback to %time% if PowerShell is unavailable
set "TIMESTR="
for /f %%a in ('powershell -NoProfile -Command "Get-Date -Format \"HHmmss\"" 2^>nul') do set "TIMESTR=%%a"
if not defined TIMESTR (
    REM Fallback: %time% format is "H:MM:SS.ss"
    set "T=%time%"
    REM remove non-digit characters
    set "T=%T::=%"
    set "T=%T:.=%"
    set "T=%T:,=%"
    set "TIMESTR=!T!"
)

set "TIMESTAMP=%YY%%MM%%DD%_%TIMESTR%"

set "RENAMED_DLL=CSharpLibrary_%TIMESTAMP%.dll"
set "RENAMED_PDB=CSharpLibrary_%TIMESTAMP%.pdb"

echo Attempting to copy from "%OUTPUT_DIR%\%ORIGINAL_DLL%" to "%OUTPUT_DIR%\%RENAMED_DLL%"

copy /Y "%OUTPUT_DIR%\%ORIGINAL_DLL%" "%OUTPUT_DIR%\%RENAMED_DLL%"
if errorlevel 1 (
    echo ERROR: Failed to copy DLL: %OUTPUT_DIR%\%ORIGINAL_DLL%
    endlocal
    exit /b 3
)

if exist "%OUTPUT_DIR%\%ORIGINAL_PDB%" (
    echo Copying PDB...
    copy /Y "%OUTPUT_DIR%\%ORIGINAL_PDB%" "%OUTPUT_DIR%\%RENAMED_PDB%"
    if errorlevel 1 (
        echo WARNING: Failed to copy PDB (continuing)
    )
) else (
    echo Notice: PDB file not found: %OUTPUT_DIR%\%ORIGINAL_PDB%
)

endlocal
exit /b 0
