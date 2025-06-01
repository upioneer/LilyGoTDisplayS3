<# 
    This script performs the following:
    - Checks for esptool
    - Gets COM ports
    - Prompts the user to select a chip for erasing flash
#>

$esptoolInstalled = python -m pip show esptool 2>$null

if (-not $esptoolInstalled) {
    Write-Host "Installing esptool..." -ForegroundColor Cyan
    python -m pip install esptool
} else {
    Write-Host "esptool detected..." -ForegroundColor Cyan
}

python.exe -m pip install --upgrade pip

Write-Host "Fetching COM ports of connected ESP32 chips..." -ForegroundColor Cyan
$chipOutput = python -m esptool chip_id 2>&1

$comPorts = @()
foreach ($line in $chipOutput) {
    if ($line -match "port\s+([A-Za-z0-9]+)") {
        $comPorts += $matches[1]
    }
}

if ($chipOutput.Count -eq 0) {
    Write-Host "No chips found." -ForegroundColor Red
    $userChoice = Read-Host "Would you like to exit the script? (Y/N)"
    if ($userChoice -eq 'Y' -or $userChoice -eq 'y') {
        Write-Host "Exiting the script..." -ForegroundColor Yellow
        exit 0
    }
}

Write-Host "Found the following chips:" -ForegroundColor Green
for ($index = 0; $index -lt $comPorts.Count; $index++) {
    Write-Host "$($index + 1): $($comPorts[$index])"
}

$userSelection = Read-Host "Enter the number of the chip you want to flash"
if ($userSelection -notmatch '^\d+$' -or $userSelection -lt 1 -or $userSelection -gt $comPorts.Count) {
    Write-Host "Invalid selection. Exiting..." -ForegroundColor Red
    exit 1
}

$selectedPort = $comPorts[$userSelection - 1]
Write-Host "You selected port: $selectedPort" -ForegroundColor Cyan

Write-Host "Erasing flash on $selectedPort..." -ForegroundColor Yellow
python -m esptool --port $selectedPort erase_flash
