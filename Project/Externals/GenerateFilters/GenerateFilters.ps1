$BasePath = $args[0]
$FiltersFilePath = $args[1]

Write-Output "BasePath: $BasePath"
Write-Output "FiltersFilePath: $FiltersFilePath"

try {
    # フィルターファイルを生成する
    $xmlHeader = @"<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">"@
    $itemGroup = "<ItemGroup>"
    $filterGroup = "</ItemGroup>"
    $xmlContent = $xmlHeader + $itemGroup + $filterGroup + "</Project>"

    Write-Output "Generated XML Content: $xmlContent" | Out-Host

    # 出力ファイルに保存
    if (-not [string]::IsNullOrEmpty($FiltersFilePath)) {
        $xmlContent | Out-File -FilePath $FiltersFilePath -Encoding UTF8
    } else {
        Write-Error "Filters file path is empty."
    }
} catch {
    Write-Error "An error occurred: $_"
}
