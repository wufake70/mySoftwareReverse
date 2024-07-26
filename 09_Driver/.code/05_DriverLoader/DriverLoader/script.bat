chcp 65001>nul
if "%~1"=="GENERATE_DLL.H_DEBUG" goto GENERATE_DLL.H_DEBUG
if "%~1"=="GENERATE_DLL.H_RELEASE" goto GENERATE_DLL.H_RELEASE
if "%~1"=="" goto END



rem 生成dll.h
:GENERATE_DLL.H_DEBUG
..\debug\sys2bin.exe ..\debug\Test.sys

:GENERATE_DLL.H_RELEASE
..\release\sys2bin.exe ..\release\Test.sys

:END