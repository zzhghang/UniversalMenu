<#
.SYNOPSIS
Starts the local UniversalMenu web preview and opens it in your browser.
.DESCRIPTION
Requires Python 3. This serves only WebPreview, and only on 127.0.0.1.
Keep the terminal open while reviewing; press Ctrl+C to stop the server.

From File Explorer: right-click this file and choose "Run with PowerShell".
Double-click works if .ps1 files on your PC are associated with PowerShell;
Windows normally opens .ps1 files in an editor instead.

From a terminal in the project directory:
  powershell -NoProfile -ExecutionPolicy Bypass -File .\WebPreview\start-preview.ps1
  powershell -NoProfile -ExecutionPolicy Bypass -File .\WebPreview\start-preview.ps1 -Port 9000

From an existing PowerShell session:
  .\WebPreview\start-preview.ps1
  .\WebPreview\start-preview.ps1 -Port 9000 -NoBrowser

You can also run: python .\WebPreview\serve.py --port 8765 --open
The default address is http://127.0.0.1:8765/.
#>
[CmdletBinding()]
param(
    [ValidateRange(1, 65535)]
    [int]$Port = 8765,
    [switch]$NoBrowser
)

$ErrorActionPreference = 'Stop'
$previewServerPath = Join-Path -Path $PSScriptRoot -ChildPath 'serve.py'
$previewPython = Get-Command 'py' -ErrorAction SilentlyContinue
$previewPythonArguments = @('-3')
if (-not $previewPython) {
    $previewPython = Get-Command 'python' -ErrorAction SilentlyContinue
    $previewPythonArguments = @()
}
if (-not $previewPython) {
    Write-Error 'Python 3 was not found. Install Python 3, then run this script again.'
    exit 1
}

$previewPythonArguments += @($previewServerPath, '--port', [string]$Port)
if (-not $NoBrowser) {
    $previewPythonArguments += '--open'
}
& $previewPython.Source @previewPythonArguments
exit $LASTEXITCODE
