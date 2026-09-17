# Host-only runner. Run from the repository root; no upload or debug connection.
$ErrorActionPreference = 'Stop'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path -LiteralPath $vswhere)) { throw 'Visual Studio C++ tools are required.' }
$vcvars = & $vswhere -latest -products '*' -find 'VC\Auxiliary\Build\vcvars64.bat' | Select-Object -First 1
if (-not $vcvars) { throw 'vcvars64.bat was not found.' }
$unityDir = '.pio\libdeps\evt\Unity\src'
if (-not (Test-Path -LiteralPath "$unityDir\unity.c")) {
    throw 'Unity is missing. Resolve the evt PlatformIO test dependency first.'
}
New-Item -ItemType Directory -Force -Path '.pio\host-test' | Out-Null
$compile = '"' + $vcvars + '" >nul && ' +
    'cl /nologo /c /TC /I"' + $unityDir + '" /Fo:.pio\host-test\unity.obj "' + $unityDir + '\unity.c" && ' +
    'cl /nologo /std:c++20 /EHsc /W4 /WX /I"' + $unityDir + '" ' +
    '/Fe:.pio\host-test\waveform.exe /Fo:.pio\host-test\waveform.obj ' +
    'test\test_pwm_do_smoke\test_waveform.cpp .pio\host-test\unity.obj && .pio\host-test\waveform.exe'
cmd /d /c $compile
if ($LASTEXITCODE -ne 0) { throw "Host tests failed: $LASTEXITCODE" }
