@echo off
setlocal enabledelayedexpansion

set "pattern=CSharpLibrary_*.dll"
set "latest="
set "latestTimestamp="

rem �ŐV�̃t�@�C�������
for %%F in (%pattern%) do (
    set "filename=%%~nF"
    set "timestamp=%%F"
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
    echo �ŐV�t�@�C����������܂���ł����B
    exit /b 1
)

echo �ŐV�t�@�C��: !latest!.dll / .pdb ���c���܂�

rem �ŐV�ȊO���폜
for %%F in (%pattern%) do (
    set "filename=%%~nF"
    if /I not "!filename!"=="!latest!" (
        echo �폜: !filename!.dll
        del "!filename!.dll" >nul 2>&1
        if exist "!filename!.pdb" (
            echo �폜: !filename!.pdb
            del "!filename!.pdb" >nul 2>&1
        )
    )
)

endlocal
exit /b 0
