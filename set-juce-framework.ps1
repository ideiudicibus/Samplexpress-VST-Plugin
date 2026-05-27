#Requires -Version 5.1
<#
.SYNOPSIS
    Configures the JUCE_FRAMEWORK environment variable for Samplexpress builds.
.DESCRIPTION
    Checks whether the JUCE_FRAMEWORK environment variable is defined.
    If missing, prompts the user for the absolute path to their local JUCE
    installation, validates it, and persists the variable in the user
    environment so that CMake and the build scripts can locate JUCE.
#>

[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"

function Test-JuceRoot {
    param([string]$Path)
    # A valid JUCE root should contain at least one of these markers
    $markers = @(
        "CMakeLists.txt",
        "Projucer.exe",
        "extras/Projucer",
        "modules/juce_core"
    )
    foreach ($m in $markers) {
        if (Test-Path (Join-Path $Path $m)) { return $true }
    }
    return $false
}

# -----------------------------------------------------------------------------
# 1. Check current value
# -----------------------------------------------------------------------------
$current = [Environment]::GetEnvironmentVariable("JUCE_FRAMEWORK", "User")
$session = $env:JUCE_FRAMEWORK

if ($current) {
    Write-Host "JUCE_FRAMEWORK is already set for this user: " -NoNewline
    Write-Host $current -ForegroundColor Cyan

    if (Test-JuceRoot $current) {
        Write-Host "Path validated — JUCE installation looks good." -ForegroundColor Green
    } else {
        Write-Warning "The existing path does not appear to contain a valid JUCE installation."
    }

    $change = Read-Host "Do you want to change it? [y/N]"
    if ($change -notmatch '^[Yy]') {
        Write-Host "Keeping existing value. Exiting." -ForegroundColor Green
        exit 0
    }
}

# -----------------------------------------------------------------------------
# 2. Prompt for path
# -----------------------------------------------------------------------------
Write-Host ""
Write-Host "Please enter the absolute path to your local JUCE installation folder." -ForegroundColor Yellow
Write-Host "Example: C:\Users\$env:USERNAME\JUCE" -ForegroundColor DarkGray
Write-Host ""

$jucePath = Read-Host "JUCE framework path"

# Trim quotes and whitespace
$jucePath = $jucePath.Trim().Trim('"', "'")

# Resolve relative paths
$jucePath = (Resolve-Path $jucePath -ErrorAction SilentlyContinue).Path
if (-not $jucePath) {
    Write-Error "The path you entered does not exist: '$($jucePath)'"
    exit 1
}

# -----------------------------------------------------------------------------
# 3. Validate
# -----------------------------------------------------------------------------
if (-not (Test-Path $jucePath)) {
    Write-Error "Path not found: $jucePath"
    exit 1
}

if (-not (Test-JuceRoot $jucePath)) {
    Write-Warning "The folder does not look like a standard JUCE installation (missing expected sub-files)."
    $continue = Read-Host "Continue anyway? [y/N]"
    if ($continue -notmatch '^[Yy]') {
        Write-Host "Aborted. No changes made." -ForegroundColor Red
        exit 1
    }
}

# -----------------------------------------------------------------------------
# 4. Persist
# -----------------------------------------------------------------------------
[Environment]::SetEnvironmentVariable("JUCE_FRAMEWORK", $jucePath, "User")
$env:JUCE_FRAMEWORK = $jucePath   # also set for current PowerShell session

Write-Host ""
Write-Host "SUCCESS: JUCE_FRAMEWORK has been set to:" -ForegroundColor Green
Write-Host "  $jucePath" -ForegroundColor Cyan
Write-Host ""
Write-Host "The variable is saved in your user environment and will be available"
Write-Host "in new Command Prompt / PowerShell sessions immediately."
Write-Host "If any existing terminals are open, restart them or run:" -ForegroundColor DarkGray
Write-Host "  `$env:JUCE_FRAMEWORK = '$jucePath'" -ForegroundColor DarkGray
