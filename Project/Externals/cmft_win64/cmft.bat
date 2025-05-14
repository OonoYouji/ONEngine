@echo off
setlocal enabledelayedexpansion

:: 入力フォルダ
set inputDir=srcTex

:: 出力先フォルダ
set outputDir=convertedTex
if not exist %outputDir% (
    mkdir %outputDir%
)

:: cmft.exe のパラメータ設定
set cmftParams=--filter none --dstFaceSize 512 --generateMipChain true --output0params "dds,rgba16f,cubemap"

:: .hdr ファイルをすべて処理
for %%f in (%inputDir%\*.hdr) do (
    set "inputFile=%%f"
    set "fileName=%%~nf"
    echo 処理中: !inputFile!

    cmft.exe ^
        --input "!inputFile!" ^
        %cmftParams% ^
        --output0 "%outputDir%\!fileName!"
)

exit
