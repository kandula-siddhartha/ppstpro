# Smart Farming runner: starts frontend server and backend app (if compiler exists)

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectRoot

Write-Host "Starting frontend on http://localhost:5500/index.html ..." -ForegroundColor Green
$serverProcess = Start-Process -FilePath "python" -ArgumentList "-m", "http.server", "5500" -WorkingDirectory $projectRoot -PassThru

Start-Sleep -Milliseconds 700
Start-Process "http://localhost:5500/index.html" | Out-Null

$gpp = Get-Command g++ -ErrorAction SilentlyContinue
$clangpp = Get-Command clang++ -ErrorAction SilentlyContinue
$cl = Get-Command cl -ErrorAction SilentlyContinue

$compiled = $false

if ($gpp) {
    Write-Host "Compiling backend with g++ ..." -ForegroundColor Cyan
    g++ -std=c++17 main.cpp -o smart_farming_app
    if ($LASTEXITCODE -eq 0) {
        $compiled = $true
        Start-Process -FilePath ".\smart_farming_app.exe" -WorkingDirectory $projectRoot
    }
} elseif ($clangpp) {
    Write-Host "Compiling backend with clang++ ..." -ForegroundColor Cyan
    clang++ -std=c++17 main.cpp -o smart_farming_app
    if ($LASTEXITCODE -eq 0) {
        $compiled = $true
        Start-Process -FilePath ".\smart_farming_app.exe" -WorkingDirectory $projectRoot
    }
} elseif ($cl) {
    Write-Host "Detected cl.exe (MSVC). Manual developer prompt may be required to compile." -ForegroundColor Yellow
} else {
    Write-Host "No C++ compiler detected. Frontend is running, but backend cannot be started yet." -ForegroundColor Yellow
    Write-Host "Install one of these tools: MinGW g++, LLVM clang++, or Visual Studio C++ Build Tools." -ForegroundColor Yellow
}

if ($compiled) {
    Write-Host "Backend started successfully." -ForegroundColor Green
}

Write-Host "Frontend server process ID: $($serverProcess.Id)" -ForegroundColor DarkGray
Write-Host "To stop frontend server later: Stop-Process -Id $($serverProcess.Id)" -ForegroundColor DarkGray
