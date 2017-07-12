# SIGNALL FOR APPLICATION

新版的信号函数，封装成旧版本的接口。
```c
int sigall(int signo, void *(*CallBack)(int))
{
  struct sigaction act, oact;
  act.sa_handler = CallBack;
  sigemptyset(&act.samask);
  act.sa_flags = 0;
  return sigaction(signo, &act, &oact);
}
```
