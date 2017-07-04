# RF 中添加python库

## 添加到系统库中
1. 在 ***C:\Python27\Lib\site-packages*** 路径下新建文件夹RJAPP
2. 在 ***RJAPP*** 文件夹下新建文件 ***\__init\__.py*** 编辑该文件：

```Python
import os
from RJAPP.keywords import *
from .filename import class_name

VERSION = '1.4.4'
__version__ = VERSION

class RJAPP(
    ken
):
    ROBOT_LIBRARY_SCOPE = 'GLOBAL'
    ROBOT_LIBRARY_VERSION = __version__

    def __init__(self):
```

```python
from RJAPP.keywords import *
"""将RJAPP文件夹下的keywords文件夹下的所有文件导入 """
from .filename import class_name
"""为导入当前路径下的filename文件中的class_name类"""、
class RJAPP(ken):
  """为RF提供的keywords，ken为对外提供的类（keywords）"""
    ROBOT_LIBRARY_SCOPE = 'GLOBAL'
    """修饰"""
```


## 采用import Library

在RF中 使用

| Command    | path/name     |arg1| arg2|
| :------------- | :------------- |
| Import Library      | MyLibrary      |
| Import Library |${CURDIR}/../Library.py |arg1|named=arg2|

1. 使用库名时，搜索路径为系统环境变量中的路径加上当前目录。
2. 使用路径时，${CURDIR}/../RJAPP 表示导入当前目录的上级目录中的RJAPP中的库。该库同上面系统库创建的一样，包含\__init\__.py等文件。
