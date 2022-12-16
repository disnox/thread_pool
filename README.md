<h2 align="center">
内核链表线程池
</h2><br>
<pre align="center">
 Build with 📷<a href="https://disnox.top">disnox</a> 
</pre>



<p align="center">
<br>
<a href="https://www.disnox.top/project">🖥 More Items</a>
<br>
<a href=" " rel="nofollow"><img src="https://img.shields.io/badge/-%F0%9F%92%BBLinux-blue"></a>
<a href=" " rel="nofollow"><img src="https://img.shields.io/badge/-%F0%9F%91%A8%E2%80%8D%F0%9F%92%BBC%2FC%2B%2B-blue"></a>
<a href=" " rel="nofollow"><img src="https://img.shields.io/badge/-%F0%9F%93%9Aproject-blue"></a>
</p>


## 1. 项目简介

采用内核链表构建的线程池，包含新建线程、新建任务、删除线程、删除任务、删除。

## 2. 软件环境

+ **VsCode** 代码编辑器
+ **Ubuntu18.04 Linux** 子系统
+ **VMware Workstation Pro** 虚拟机
+ **MobaXterm_Personal_20.3** 或者 **SecureCRT** SSH 软件

## 3. 项目使用

在终端文件下使用命令

```C
xx@xx:xxx/project$ ./a.out
```

## 4. 知识点使用

+ **Linux 内核链表：**将数据域和指针域分开，将车次信息、购票信息、用户信息挂载到 Linux 内核链表上。
+ **标准IO函数使用：**使用 函数 **fopen、fgets、fscanf、fprintf、fclose** 打开信息文件读取信息、输入信息并关闭文件。
+ **内存空间：**使用 **malloc** 函数分配堆内存空间。
+ **线程函数的使用：**创建线程，取消线程，线程接合。

## 5. 目录结构描述

```bash
├── README.md     		# 项目简介
├── a.out        		# 可执行文件
├── kernel_list.h		# 内核链表
├── main.c       		# 主函数文件
├── thread_pool.c		# 内核链表构建线程池函数文件
└── thread_pool.h		# 内核链表构建线程池函数头文件
```
