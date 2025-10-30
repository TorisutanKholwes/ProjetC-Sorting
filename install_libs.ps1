param(
    [string]$InstallDir = "$(Join-Path $PSScriptRoot 'lib')"
)

Write-Host "Installing SDL3 and dependencies into $InstallDir..."

if (-not (Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Path $InstallDir | Out-Null
}

$InstallFullDir = (Resolve-Path $InstallDir).Path

$repos = @(
    "https://github.com/libsdl-org/SDL.git",
    "https://github.com/libsdl-org/SDL_image.git",
    "https://github.com/libsdl-org/SDL_mixer.git",
    "https://github.com/libsdl-org/SDL_ttf.git"
)

foreach ($repo in $repos) {
    $name = [System.IO.Path]::GetFileNameWithoutExtension($repo)
    $srcDir = Join-Path $InstallDir $name
    $buildDir = Join-Path $srcDir "build"

    Write-Host "`n=== Installing $name ==="
    if (-not (Test-Path $srcDir)) {
        git clone --recurse-submodules $repo $srcDir
        if ($LASTEXITCODE -ne 0) {
            throw "Git clone failed for $name"
        }
    }

    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir | Out-Null
    }

    Push-Location $buildDir
    cmake -DCMAKE_INSTALL_PREFIX="$InstallFullDir" -DCMAKE_BUILD_TYPE=Release -DSDL3_DIR="$InstallFullDir\\cmake" ..
    if ($LASTEXITCODE -ne 0) {
        Pop-Location
        throw "CMake configuration failed for $name"
    }

    cmake --build . --config Release --target install
    if ($LASTEXITCODE -ne 0) {
        Pop-Location
        throw "Build/install failed for $name"
    }
    Pop-Location

    Remove-Item -Path "$srcDir" -Recurse -Force
}

Write-Host "`nSDL3 installation complete in $InstallDir"
exit 0
