@echo off
chcp 65001 >nul 2>&1  # 解决中文乱码
echo ==============================================
echo 注册 COM 组件（非静默模式，显示系统弹窗）
echo 脚本执行目录：%~dp0
echo ==============================================

:: 定义注册工具和组件路径（绝对路径，避免歧义）
set "regTool=C:\Windows\SysWOW64\regsvr32.exe"
set "comPath=%cd%\com\"

:: 注册每个组件（去掉 /s，让系统弹出成功/失败弹窗）
echo 1. 注册 grdes50.dll
%regTool% "%comPath%grdes50.dll"
if %errorlevel% equ 0 (echo [日志] grdes50.dll 注册返回码：0（成功）) else (echo [日志] grdes50.dll 注册返回码：%errorlevel%（失败）)

echo.
echo 2. 注册 gregn50.dll
%regTool% "%comPath%gregn50.dll"
if %errorlevel% equ 0 (echo [日志] gregn50.dll 注册返回码：0（成功）) else (echo [日志] gregn50.dll 注册返回码：%errorlevel%（失败）)

echo.
echo 3. 注册 vsflex8.ocx
%regTool% "%comPath%vsflex8.ocx"
if %errorlevel% equ 0 (echo [日志] vsflex8.ocx 注册返回码：0（成功）) else (echo [日志] vsflex8.ocx 注册返回码：%errorlevel%（失败）)

echo.
echo 4. 注册 msado27.tlb（TLB 类型库需用 /i 参数！）
regsvr32.exe /i "%comPath%msado27.tlb"  
if %errorlevel% equ 0 (echo [日志] msado27.tlb 注册返回码：0（成功）) else (echo [日志] msado27.tlb 注册返回码：%errorlevel%（失败）)

echo.
echo ==============================================
echo 执行完成！请查看弹窗提示和日志
echo ==============================================
pause