param(
    [Parameter(Mandatory=$true, Position=0)]
    [string]$NamespaceName,
    
    [Parameter(Mandatory=$false, Position=1)]
    [string]$TargetDirectory = "."
)

# Validate target directory
if (-not (Test-Path $TargetDirectory)) {
    Write-Error "Target directory not found: $TargetDirectory"
    exit 1
}

# Process .h files
Get-ChildItem -Path $TargetDirectory -Filter "*.h" -Recurse | ForEach-Object {
    $filePath = $_.FullName
    Write-Host "Processing: $filePath"
    
    $lines = Get-Content $filePath
    
    if ($lines.Count -lt 1) {
        Write-Warning "Skipped: $filePath (File is empty)"
        return
    }
    
    # Find the first class or struct declaration
    $classLineIndex = -1
    $templateLineIndex = -1
    $namespaceLineIndex = -1
    
    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        
        # Skip comment lines
        if ($line -match '^\s*//' -or $line -match '^\s*namespace\b') {
            continue
        }
        
        # Detect namespace declaration
        if ($namespaceLineIndex -eq -1 -and $line -match '^namespace\s*{') {
            $namespaceLineIndex = $i
            Write-Host "  Namespace detected: $($line.Trim())"
        }
        
        # Detect template declaration
        if ($templateLineIndex -eq -1 -and $line -match '^\s*template\s*<') {
            $templateLineIndex = $i
            Write-Host "  Template detected: $($line.Trim())"
        }
        
        # Detect class or struct declaration (supports `final`)
        if ($line -match '^\s*(class|struct)\s+[\w<>]+(\s+final)?\s*(:|\{|;|$)') {
            $classLineIndex = $i
            Write-Host "  Class/Struct detected: $($line.Trim())"
            break
        }
    }
    
    if ($classLineIndex -eq -1) {
        Write-Warning "Skipped: $filePath (Class/Struct declaration not found)"
        return
    }
    
    # Determine insertion position
    # Priority: namespace > template > class
    $insertLineIndex = $classLineIndex
    
    if ($namespaceLineIndex -ne -1 -and $namespaceLineIndex -lt $classLineIndex) {
        $insertLineIndex = $namespaceLineIndex
        Write-Host "  Insert position: Before existing namespace declaration (line $($insertLineIndex + 1))"
    } elseif ($templateLineIndex -ne -1 -and $templateLineIndex -lt $classLineIndex) {
        $insertLineIndex = $templateLineIndex
        Write-Host "  Insert position: Before template declaration (line $($insertLineIndex + 1))"
    } else {
        Write-Host "  Insert position: Before class declaration (line $($insertLineIndex + 1))"
    }
    
    # Build new file content
    $newLines = @()
    
    if ($insertLineIndex -gt 0) {
        $newLines += $lines[0..($insertLineIndex - 1)]
    }
    
    # Add namespace block
    $newLines += "namespace $NamespaceName {"
    $newLines += ""
    
    # Add rest of file
    $newLines += $lines[$insertLineIndex..($lines.Count - 1)]
    
    # Append closing namespace
    $newLines += ""
    $newLines += "} /// $NamespaceName"
    
    # Write file
    $newLines | Set-Content $filePath -Encoding UTF8
}

# Process .cpp files
Get-ChildItem -Path $TargetDirectory -Filter "*.cpp" -Recurse | ForEach-Object {
    $filePath = $_.FullName
    Write-Host "Processing: $filePath"
    
    $lines = Get-Content $filePath
    
    if ($lines.Count -lt 1) {
        Write-Warning "Skipped: $filePath (File is empty)"
        return
    }
    
    # Get base name without extension
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($filePath)
    
    # Locate matching .h include line
    $includeLineIndex = -1
    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match "^\s*#include\s+[<""].*$baseName\.h[>""]") {
            $includeLineIndex = $i
            Write-Host "  Include detected: $($lines[$i].Trim())"
            break
        }
    }
    
    if ($includeLineIndex -eq -1) {
        Write-Warning "Skipped: $filePath (Matching header include not found)"
        return
    }
    
    # Build new file content
    $newLines = @()
    
    $newLines += $lines[0..$includeLineIndex]
    
    # Insert using namespace
    $newLines += ""
    $newLines += "using namespace $NamespaceName;"
    
    if ($includeLineIndex -lt $lines.Count - 1) {
        $newLines += $lines[($includeLineIndex + 1)..($lines.Count - 1)]
    }
    
    # Write file
    $newLines | Set-Content $filePath -Encoding UTF8
}

Write-Host "Completed."
