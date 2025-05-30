#pragma once
#include <windows.h>
#include <cstdio>

#define OUTMESSAGE(a,b) printf("%-36s %s\n",a,b ? "Detected." : "ok")

// PEB的四个标志位
void _PEBFunc();

// 检查peb标志位
void _IsDebuggerPresent();

// 确定指定的进程是否正在被调试。
void _CheckRemoteDebuggerPresent();

// 查询当前系统是否被内核调试
void _NtQuerySystemInformation();

/*
原理，NtClose函数在释放无效句柄时。如果没有被调试，那么函数返回FALSE。
如果处于调试状态，则会抛出异常C0000008H。
*/
void _NtClose();

/*
进内核查询 ProcessDebugPort、ProcessDebugObjectHandle、ProcessDebugFlags
*/
void _NtQueryInformationProcess();

// 在调试器中隐藏当前线程，断点失效
void _ThreadHideFromDebugger();

/*
复制后并关闭受保护的句柄 触发异常

调试器对异常处理的影响
	有调试器时：
		调试器优先捕获异常（如 Visual Studio 默认拦截所有异常）。
		如果调试器选择“继续执行”，异常会传递给应用程序的 SEH 处理。
	无调试器时：
		SEH 不能捕获所有类型的异常或错误,
		系统可能直接返回错误码（而非触发异常），导致 SEH 未被触发。
		此时即使操作失败，也不会进入 __except 块。
*/
void _NtDuplicateObject();

/*
查询是否存在调式对象，不准确
*/
void _NtQueryObject();