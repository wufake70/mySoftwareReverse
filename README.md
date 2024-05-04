"# 软件逆向(滴水三期)" 

一、.code 文件夹存放作业源码,(win32之前的使用vc++6.0环境，之后使用vs2022环境)
二、三个项目完成两个(PEtool、加密壳)


├─01_Assembly
│  ├─000_画函数调用堆栈图
│  └─img
├─02_CLanguage
├─03_PE
│  └─.CODE
│      ├─005_AddMessageBoxToExe
│      ├─006_操作节
│      │  ├─1_新增节
│      │  ├─2_合并节
│      │  ├─3_扩大节
│      │  └─4_目录项
│      ├─009_导出表
│      │  ├─01_输出导出表
│      │  └─02_getFunAddrByOrdinals
│      ├─010_重定向表
│      ├─011_移动表
│      │  ├─1_导出表
│      │  ├─2_重定位表
│      │  └─3_模拟系统通过重定位表修复绝对地址
│      ├─012_导入表
│      ├─013_绑定导入表
│      ├─014_导入表注入dll
│      │  ├─x64_vs2022
│      │  │  └─x64
│      │  │      ├─Debug
│      │  │      │  └─mydll.tlog
│      │  │      └─Release
│      │  │          └─mydll.tlog
│      │  └─x86_vc6.0
│      ├─015_输出pe资源表
│      └─016_vs2022
├─04_CPlusPlus_DataStructures
│  └─.code
│      ├─001_Vector实现
│      ├─002_单链表实现
│      └─003_数据结构(BST)
└─05_Win32Api
    └─.code
        ├─001_win32调试输出工具
        ├─002_消息断点测试
        ├─003_icon
        ├─004_输出pe资源表(vc++6.0)
        ├─005_WindowsProject1
        ├─006_遍历进程和模块
        ├─007_PETool
        │  ├─Debug
        │  │  └─PETool.tlog
        │  └─x64
        │      └─Debug
        │          └─PETool.tlog
        ├─008_Thread
        │  └─Debug
        │      └─WinDesktopTest.tlog
        ├─009_Thread_Mutex
        ├─010_MakerConsumer
        ├─011_Process
        ├─012_创建被挂起的进程
        │  ├─Debug
        │  │  └─WinDesktopTest.tlog
        │  └─x64
        │      └─Debug
        │          └─WinDesktopTest.tlog
        ├─013_Packing
        │  ├─PETool
        │  │  ├─Debug
        │  │  │  └─PETool.tlog
        │  │  ├─Release
        │  │  │  └─PETool.tlog
        │  │  └─x64
        │  │      └─Debug
        │  │          └─PETool.tlog
        │  └─Shell
        │      └─Debug
        ├─014_InjectDll(x64)
        │  └─x64
        │      ├─Debug
        │      │  └─InjectDll.tlog
        │      └─Release
        │          └─InjectDll.tlog
        ├─014_RemoteThreadInjectDll(PETool)
        │  └─Debug
        │      └─PETool.tlog
        ├─015
        │  └─Debug
        │      └─PETool.tlog
        ├─016_IATHook
        │  ├─0_hookItself
        │  │  └─x64
        │  │      └─Debug
        │  │          └─IATHook.tlog
        │  └─1_hookExplorer
        ├─017_KeyMonitor
        └─018_PETool(x64)
            └─x64
                ├─Debug
                │  └─PETool.tlog
                └─Release
                    └─PETool.tlog