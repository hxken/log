# 守护进程
守护进程一般用shell脚本调用执行。程序末尾通常加***d***表示该程序为deamon程序。

# 守护进行
守护进程的步骤如下：  
-  ***fork*** 后，父进程退出，子进程调用***setsid***取消与任何终端的关联。
- 用 ***chdir***将根目录设为子进程的当前目录，防止由于进程的占有，是的占有的磁盘无法弹出。
- 设置进程的 ***umask***为 0
- 子进程关闭从父进程继承来的文件描述符，如：
```
STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO
```


# 守护进程的shell脚本
守护进程由shell脚本通过发送信号等形式，实现对deamon程序的控制。
```shell
WHOAMI = `whoami`
PID = `PS -US$WHOAMI | grep abc | awk '{print $1}'`
if (test "$PID" != "") then
    kill $PID
fi
```
