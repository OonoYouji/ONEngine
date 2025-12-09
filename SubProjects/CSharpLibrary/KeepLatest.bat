@echo off
setlocal enabledelayedexpansion

set "pattern=CSharpLibrary_*.dll"

rem --- Check if any matching files exist ---
dir /b "%pattern%" >nul 2>&1
if errorlevel 1 (
    echo No matching DLL files found.
    endlocal
    exit /b 1
)

set "latest="
set "latestTimestamp="

rem --- Find the latest DLL based on timestamp in filename ---
for %%F in (%pattern%) do (
    set "filename=%%~nF"
    set "timestamp=!filename:CSharpLibrary_=!"
    set "timestamp=!timestamp:.dll=!"
    set "timestamp=!timestamp:_=!"
    if not defined latestTimestamp (
        set "latestTimestamp=!timestamp!"
        set "latest=%%~nF"
    ) else (
        if "!timestamp!" GTR "!latestTimestamp!" (
            set "latestTimestamp=!timestamp!"
            set "latest=%%~nF"
        )
    )
)

if not defined latest (
    echo Could not determine the latest DLL file.
    endlocal
    exit /b 1
)

echo Latest DLL found: !latest!.dll

rem --- Delete older DLLs and PDBs ---
for %%F in (%pattern%) do (
    set "filename=%%~nF"
    if /I not "!filename!"=="!latest!" (
        echo Deleting: !filename!.dll
        del /Q "!filename!.dll" >nul 2>&1
        if exist "!filename!.pdb" (
            echo Deleting: !filename!.pdb
            del /Q "!filename!.pdb" >nul 2>&1
        )
    )
)

endlocal
exit /b 0
