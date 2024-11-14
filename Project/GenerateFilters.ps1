# .sln ファイルの名前を取得
$slnFile = Get-ChildItem -Path $PSScriptRoot -Filter *.sln | Select-Object -First 1
if ($slnFile) {
    $projectName = [System.IO.Path]::GetFileNameWithoutExtension($slnFile.Name)
} else {
    Write-Host "Error: No .sln file found in the current directory."
    exit 1
}

# 出力ファイル名の設定
$outputFile = "$projectName.vcxproj.filters"

# .filters ファイルのヘッダー
$header = @"
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
"@

# .filters ファイルのフッター
$footer = "</Project>"

# フォルダ構造を取得し、フィルタ要素を作成
function Get-Filters {
    param ([string]$rootDirectory)
    $filters = @()
    Get-ChildItem -Path $rootDirectory -Recurse -Directory | ForEach-Object {
        $relativePath = $_.FullName.Substring($rootDirectory.Length).TrimStart("\")
        $filter = @"
    <Filter Include="$relativePath">
        <UniqueIdentifier>{$([guid]::NewGuid().ToString())}</UniqueIdentifier>
    </Filter>
"@
        $filters += $filter
    }
    return $filters
}

# ソースファイルを取得し、ファイル要素を作成
function Get-FileEntries {
    param ([string]$rootDirectory)
    $fileEntries = @()
    Get-ChildItem -Path $rootDirectory -Recurse -File -Include *.cpp, *.h | ForEach-Object {
        $relativePath = $_.FullName.Substring($rootDirectory.Length).TrimStart("\")
        $directoryName = Split-Path $relativePath -Parent
    $fileEntry = @"
     <ClCompile Include="$relativePath">
         <Filter>$directoryName</Filter>
     </ClInclude>
"@ 
        if ($_.Extension -eq ".h") {
            $fileEntry = $fileEntry -replace "ClCompile", "ClInclude"
        }
        $fileEntries += $fileEntry
    }
    return $fileEntries
}

#   $fileEntry = @"
#    <ClCompile Include="$relativePath">
#        <Filter>$directoryName</Filter>
#    </ClInclude>
#"@ 


# フォルダのパスをスクリプトのディレクトリに設定
$rootDirectory = $PSScriptRoot

# .filters ファイルを作成
$filters = Get-Filters -rootDirectory $rootDirectory
$fileEntries = Get-FileEntries -rootDirectory $rootDirectory

# ファイル出力
# Set-Content -Path $outputFile -Value ($header + $filters + $fileEntries + $footer)
# ファイル出力
Set-Content -Path $outputFile -Value ($header + "`n" + ($filters -join "`n") + "`n" + ($fileEntries -join "`n") + "`n" + $footer)

Write-Host "Visual Studio filters file generated: $outputFile"
