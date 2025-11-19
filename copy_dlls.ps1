# Script to copy SFML DLLs to the executable directory
$sfmlBin = "C:\SFML\bin"
$targetDir = ".\build\Release"

if (-not (Test-Path $targetDir)) {
    Write-Host "Error: Release directory not found at $targetDir"
    Write-Host "Please build the project first."
    exit 1
}

if (-not (Test-Path $sfmlBin)) {
    Write-Host "Error: SFML bin directory not found at $sfmlBin"
    Write-Host "Please install SFML or update the path in this script."
    exit 1
}

Write-Host "Copying SFML DLLs from $sfmlBin to $targetDir..."

# Copy all SFML DLLs
Copy-Item "$sfmlBin\sfml-*.dll" -Destination $targetDir -Force

Write-Host "`nDLLs copied successfully:"
Get-ChildItem "$targetDir\sfml-*.dll" | ForEach-Object { Write-Host "  - $($_.Name)" }

Write-Host "`nYou can now run: .\build\Release\RubikGame.exe"

