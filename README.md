# windows_ls

## 参数指南（记得要给本程序创建环境变量）

### -l	以长格式列出文件信息，包括权限、大小、修改日期和时间等

### -a	显示所有文件，包括隐藏文件（. 和 ..）

### -R	递归地列出指定目录及其子目录中的文件

### -x	横向排布文件，一行一行地列出文件

### -s	按文件大小排序

### -d	仅显示目录

### -t	按最后修改时间排序

### --version 显示版本号、作者信息和开源地址

### --help	显示帮助信息

### 支持特定文件的高亮显示，支持组合参数（除了-s和-t一起用）

## 演示

```bash
ls
```

![1734353422615](images/README/1734353422615.png)

```bash
ls -l
```

![1734353654083](images/README/1734353654083.png)

```bash
ls -ls
```

![1734353704495](images/README/1734353704495.png)

```bash
ls -lt
```

![1734353729550](images/README/1734353729550.png)

```bash
ls -lR 
```

![1734353760878](images/README/1734353760878.png)

```bash
ls -lRs
```

![1734353823387](images/README/1734353823387.png)

```bash
ls -lRt
```

![1734353861329](images/README/1734353861329.png)

```bash
ls -R
```

![1734353979585](images/README/1734353979585.png)

```bash
ls -Rt
```

![1734354005424](images/README/1734354005424.png)

```bash
ls -Rs
```

![1734354033207](images/README/1734354033207.png)

```bash
ls --version
```

![1734354107182](images/README/1734354107182.png)

```bash
ls --help
```

![1734354169234](images/README/1734354169234.png)
