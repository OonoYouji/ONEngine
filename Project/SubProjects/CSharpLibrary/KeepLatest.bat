@echo off
setlocal enabledelayedexpansion

set "pattern=CSharpLibrary_*.dll"
set "latest="
set "latestTimestamp="

rem 最新のファイルを特定
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
    echo 最新ファイルが見つかりませんでした。
    exit /b 1
)

echo 最新ファイル: !latest!.dll / .pdb を残します

rem 最新以外を削除
for %%F in (%pattern%) do (
    set "filename=%%~nF"
    if /I not "!filename!"=="!latest!" (
        echo 削除: !filename!.dll
        del "!filename!.dll" >nul 2>&1
        if exist "!filename!.pdb" (
            echo 削除: !filename!.pdb
            del "!filename!.pdb" >nul 2>&1
        )
    )
)

endlocal
exit /b 0
