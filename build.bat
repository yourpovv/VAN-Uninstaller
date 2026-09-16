@echo off
setlocal

set "ROOT=%~dp0"
set "BUILD=%ROOT%build"

if not exist "%BUILD%" mkdir "%BUILD%"

pushd "%ROOT%"
windres resource.rc -O coff -o "%BUILD%\resource.res"
set "RC_RESULT=%errorlevel%"
popd
if not %RC_RESULT%==0 (
    echo Resource compile failed - is windres on PATH?
    pause
    exit /b 1
)

gcc -std=c11 -O2 -Wall -Wextra ^
    -specs="%ROOT%no-default-manifest.spec" ^
    -finput-charset=UTF-8 -fexec-charset=UTF-8 ^
    -I"%ROOT%include" ^
    "%ROOT%src\main.c" "%ROOT%src\banner.c" "%ROOT%src\console.c" ^
    "%ROOT%src\log.c" "%ROOT%src\vanguard.c" "%ROOT%src\color.c" ^
    "%BUILD%\resource.res" ^
    -o "%BUILD%\VAN-Uninstaller.exe" ^
    -mconsole -lshell32
if errorlevel 1 (
    echo Build failed - is gcc on PATH?
    pause
    exit /b 1
)

echo %BUILD%\VAN-Uninstaller.exe
