@echo off
rem =====================================================================
rem r.bat  compile -> run test case (Windows version of util/r)
rem   Command Prompt : r      /  r c
rem   PowerShell     : ./r    /  ./r c     (.\r c also works)
rem   r    : run a.exe without compiling (uses the last compiled a.exe)
rem   r c  : compile A.cpp, then run (any argument means compile)
rem   input: input.txt -> output: output.txt, debug output (cerr / debug): debug.txt
rem   -DLOCAL is added, so debug(...) in the template is enabled
rem   ACL (atcoder/) is assumed to be on g++'s include path
rem   ASCII only on purpose (Japanese text in .bat breaks with some code pages)
rem =====================================================================
setlocal
cd /d "%~dp0"
set "SRC=A.cpp"
set "BIN=a.exe"

if "%~1"=="" goto run
echo [compile] %SRC%
rem --stack: Windows default stack is 1 MB, so deep recursion would crash. Make it 256 MB like AtCoder.
g++ -std=gnu++20 -O2 -DLOCAL -Wall -Wextra -Wl,--stack,268435456 -o "%BIN%" "%SRC%"
if errorlevel 1 (
    echo [compile] error
    exit /b 1
)

:run
if not exist "%BIN%" (
    echo %BIN% not found. Compile first: r c
    exit /b 1
)
if not exist input.txt type nul > input.txt

set "T0=%time%"
"%BIN%" < input.txt > output.txt 2> debug.txt
set "ST=%errorlevel%"
set "T1=%time%"
call :tocs "%T0%" C0
call :tocs "%T1%" C1
set /a D=C1-C0
if %D% lss 0 set /a D+=8640000
set /a S=D/100, CS=D%%100
if %CS% lss 10 set "CS=0%CS%"

if "%ST%"=="0" (
    echo [run] OK  %S%.%CS% s
) else (
    echo [run] runtime error ^(exit code %ST%^)  %S%.%CS% s
)
echo ----- output.txt -----
set "LINES=0"
for /f "tokens=1* delims=:" %%a in ('findstr /n "^" output.txt') do (
    if %%a leq 30 echo(%%b
    set "LINES=%%a"
)
if %LINES% gtr 30 echo ...^(see output.txt for the rest^)
for %%F in (debug.txt) do if %%~zF gtr 0 echo ^(debug.txt has debug output^)
exit /b 0

rem :tocs "H:MM:SS.cc" VAR  -> VAR = time in 1/100 s
:tocs
set "t=%~1"
set "t=%t: =0%"
if "%t:~1,1%"==":" set "t=0%t%"
set /a "%2=((1%t:~0,2%-100)*3600+(1%t:~3,2%-100)*60+(1%t:~6,2%-100))*100+(1%t:~9,2%-100)"
exit /b
