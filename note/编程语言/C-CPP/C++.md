# C++
接口 纯虚函数
virtual修饰函数 并且等于0
```cpp
class interface
{
public:
  virtual int test(int test) = 0;
}
```
接口无法直接被实例化，必须被继承。
