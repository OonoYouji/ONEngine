@echo off
setlocal
:: Move to repository root (two levels up from Project/Tools)
pushd %~dp0..\..

:: Check if python is available, otherwise try python3
where python >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    python Project/Tools/codegen.py
) else (
    where python3 >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        python3 Project/Tools/codegen.py
    ) else (
        echo Error: Python not found. Please install Python and add it to your PATH.
    )
)

popd
pause
endlocal
