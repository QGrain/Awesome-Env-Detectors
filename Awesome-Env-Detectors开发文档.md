# Awesome-Env-Detectors开发文档

| 项目总览     | 说明                                                         |
| ------------ | ------------------------------------------------------------ |
| 编程语言     | **C**/C++                                                    |
| 跨平台支持   | Windows，Linux                                               |
| 团队协作     | **git**，wechat ​ :dog2:                                      |
| **进度安排** | 总时长10天：10月20日-10月29日<br>10.20：完成相关知识学习，任务分工，项目环境搭建<br>10.21-10.23：完成日志模块，完成VD和DD的demo代码<br>10.24-10.28：完成多维度检测代码和输出显示模块<br>10.29：完成集成测试<br>10.25-10.29：完成演示实验和PPT制作 |
| 项目输出     | 源码，演示程序，PPT报告                                      |
| 参考资料     | 1. https://github.com/LordNoteworthy/al-khaser<br>2. [反虚拟机技术总结](https://zhuanlan.zhihu.com/p/35423785)<br>3. [详解反虚拟机技术](https://blog.csdn.net/qq_32400847/article/details/52830990)<br>4. [[看雪原创]反调试技术总结](https://bbs.pediy.com/thread-225740.htm)<br>5. [反调试&反反调试&OD插件的编写](https://knocked.github.io/2020/01/02/%E5%8F%8D%E8%B0%83%E8%AF%95&%E5%8F%8D%E5%8F%8D%E8%B0%83%E8%AF%95&OD%E6%8F%92%E4%BB%B6%E7%9A%84%E7%BC%96%E5%86%99/)<br>6. [腾讯2016VD](https://github.com/houjingyi233/test-virtual-machine/)\|[腾讯2016DD](https://github.com/houjingyi233/test-debug/) 两个开源仓库<br>7. ... |

## 1 VirtualEnv-Detector模块

> **考虑到实验展示的便捷性，目前暂定支持以下常见虚拟环境的检测：**
>
> - VMware
> - Docker

### 1.1 支持VMware

#### 1.1.1 检测Windows虚拟环境

**（1）检测VMware特定服务**

| 服务名             | 显示名称                   | 可执行文件路径                                               | 是否存在于VM | 是否存在于REAL |
| ------------------ | -------------------------- | ------------------------------------------------------------ | ------------ | -------------- |
| VMwareHostd        | ...                        | C:\Program Files (x86)\VMware\VMware Workstation\vmware-hostd.exe | 否           | 是             |
| VMnetDHCP          | ...                        | C:\WINDOWS\SysWOW64\vmnetdhcp.exe                            | 否           | 是             |
| VMware NAT Service | ...                        | C:\WINDOWS\SysWOW64\vmnat.exe                                | 否           | 是             |
| VMUSBArbService    | ...                        | C:\Program Files (x86)\Common Files\VMware\USB\vmware-usbarbitrator64.exe | ？           | 是             |
| VGAuthService      | ...                        | C:\Program Files\VMware\VMware Tools\VMware VGAuth\VGAuthService.exe | 是           | ？             |
| vmvss              | VMware Snapshot Provider   | ？                                                           | 是           | ？             |
| vm3dservice        | VMware SVGA Helper Service | C:\Windows\system32\vm3dservice.exe                          | 是           | 否             |
| VMTools            | VMware Tools               | C:\Program Files\VMware\VMware Tools\vmtoolsd.exe            | 是           | 否             |
| ...                | ...                        | ...                                                          | ...          | ...            |

**特别说明：**

- 上表内容有待负责这一部分的同学完善，可以查阅资料、对比Win虚拟机和实体机的服务差别来完善此表`...`和`?`部分
- 可执行文件路径包含VMware安装路径的并不固定，在编写检测代码时需要考虑是否写死or用环境变量来替代



**（2）检测VMware特定进程**

- 有待进一步调研
- 感觉和（1）有些重复

**（3）检测VMware特定硬件信息**

- **MAC地址**：通常，MAC地址的前三个字节标识一个提供商。以00:05:69、00:0c:29和00:50:56开始的MAC地址与VMware相对应；以00:03:ff开始的MAC地址与virtualpc对应；以08:00:27开始的MAC地址与virtualbox对应
- **基于主板序列号、主机型号、系统盘所在磁盘名称等其他硬件信息**

**（4）检测VMware特定注册表信息**

- 如针对VMware可以判断注册表项HKEY_CLASSES_ROOT\\Applications\\VMwareHostOpen.exe

- 有待进一步调研，可参考al-khaser源码实现

**（5）检测时间差**

- 由于在虚拟机中，代码的运行速度通常不如真实主机，故可通过运行一段特定的代码来比较运行时间差来判断是否处于虚拟机之中
- （准确度有待考量，因为运行速度较为依赖硬件环境）



#### 1.1.2 检测Linux虚拟环境

**（1）检测VMware特定服务**

- 如检测`VMware Tools`参考**1.1.1（1）**



### 1.2 支持Docker

- 通过`systemd-detect-virt -c`命令判断，除非容器带有`systemd`
- 通过`pid=1`的进程判定，除非容器以`/sbin/init`启动
- 通过是否拥有内核文件判定，除非容器挂在了宿主机的`/boot`
- 通过`/proc/1/cgroup`是否含有`docker或rkt`等关键字来判定
- 通过是否存在`/.dockerenv`文件来判定



时间有限，上述判定方法较为局限，有待进一步调研



### 1.3 支持其它虚拟环境

- 受时间限制，以下支持可以选做
	- VirtualBox
	- QEMU, KVM
	- HyperV
	- Parallel
	- XEN
	- Wine
- 不过显然来不及做（orz）
- 如果时间真的不够的话，**1.2 支持Docker**也可以不做了 :bee:

## 2 DebugEnv-Detector模块

> 调试环境检测可以分为以下两种：
>
> - 检测自身进程是否正在被调试
> - 检测其它进程是否正在被调试（optional）

以下检测维度，选取相对好实现的，疑义度相对低的即可

### 2.1 通过Windows API来检测

#### 2.1.1 IsDebugPresent

#### 2.1.2 CheckRemoteDebuggerPresent

#### 2.1.3 NtQueryInformationProcess

#### 2.1.4 GetLastError

### 2.2 通过调试工具系统痕迹来检测

#### 2.2.1 调试器引用的注册表

#### 2.2.2 查找窗体信息

#### 2.2.2 查找进程信息

### 2.3 通过调试器行为来检测

#### 2.3.1 检查软件断点

#### 2.3.2 检查硬件断点

#### 2.3.3 检查程序执行时间差

#### 2.3.4 检查程序父进程

### 2.4 退出机制

- 如果检测到自身进程正在被调试，立刻退出
- 如果检测到其它进程正在被调试，kill（需要权限）

## 3 输出标准定义

### 3.1 日志输出标准

#### 3.1.1 日志格式

```bash
# 调试信息，用于调试功能模块，程序release后需关闭
[DEBUG][TIMESTAMP] DEBUG_MSG

# 提示信息，用于打印给用户提示信息
[INFO][TIMESTAMP] INFO_MSG

# 告警信息，用于告警用户诸如环境配置不当等问题
[WARN][TIMESTAMP] WARN_MSG

# 报错信息，用于提示用户程序错误
[ERROR][TIMESTAMP] ERROR_MSG

# 其中TIMESTAMP为时间戳，精确到秒，暂定如下
HH:MM:SS
```

#### 3.1.2 日志输出方式：

- 输出到stdout
- 输出到run.log

### 3.2 检测结果输出标准

#### 3.2.1 命令行输出

- 将检测项目及其结果输出到stdout
- 输出格式如下：

```bash
# 还没想好，可参考https://github.com/LordNoteworthy/al-khaser的输出格式
```

#### 3.2.2 图形化输出（optional，视时间人手情况而定）

- 在拥有桌面系统的环境里，可以图形化输出检测结果
- 输出格式待定
	- 如Windows下弹出一个message box
	- 如Linux下采用zenity或notify-send或whiptail或dialog实现弹窗（美观性由高到低）

## 4 实验设计

### 4.1 虚拟环境检测实验

- VMware **[用于课堂演示]**
	- Windows 7
	- Ubuntu 18.04

- Docker
	- CentOS 7
- 云服务器
	- Ubuntu 18 on Aliyun
	- Windows Server 2008（有条件的话）

### 4.2 调试环境检测实验

- 自身进程调试检测
	- OllyDbg **[用于课堂演示]**
	- x32dbg/x64dbg

- 其它进程调试检测
	- 同上