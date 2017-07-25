# C调用JAVA

步骤：
1. 创建虚拟机  
```c
JNI_CreateJavaVM()
```
2. 获得class  
```c
(*env)->GetMethodID()
```

3. 实例化对象  
```c
(*env)->NewObject()
```

4. 调用方法  
```c  
(*env)->CallStaticVoidMethod() //调用静态函数  
(*env)->CallIntMethod() //非静态函数
```

----
```
javap -p -s filename.class
```

## 读取或者设置类中的属性

```c
(*env)->GetFieldld()  //  
(*env)->SetObjectField()
(*env)->SetIntField()
```
