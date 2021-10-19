# Awesome-Env-Detectors开发文档

| 项目总览     | 说明                                                         |
| ------------ | ------------------------------------------------------------ |
| 编程语言     | **C**/C++                                                    |
| 跨平台支持   | Windows，Linux                                               |
| 团队协作     | **git**，wechat ​ :dog2:                                      |
| **进度安排** | 总时长10天：10月20日-10月29日<br>10.20：完成相关知识学习，任务分工，项目环境搭建<br>10.21-10.23：完成日志模块，完成VD和DD的demo代码<br>10.24-10.28：完成多维度检测代码和输出显示模块<br>10.29：完成集成测试<br>10.25-10.29：完成演示实验和PPT制作 |
| 项目输出     | 源码，演示程序，PPT报告                                      |
| 参考资料     | 1. https://github.com/LordNoteworthy/al-khaser<br>2. [反虚拟机技术总结](https://zhuanlan.zhihu.com/p/35423785)<br>3. [详解反虚拟机技术](https://blog.csdn.net/qq_32400847/article/details/52830990)<br>4. [[看雪原创]反调试技术总结](https://bbs.pediy.com/thread-225740.htm)<br>5. [反调试&反反调试&OD插件的编写](https://knocked.github.io/2020/01/02/%E5%8F%8D%E8%B0%83%E8%AF%95&%E5%8F%8D%E5%8F%8D%E8%B0%83%E8%AF%95&OD%E6%8F%92%E4%BB%B6%E7%9A%84%E7%BC%96%E5%86%99/)<br>6. ... |

## VirtualEnv-Detector模块

> **考虑到实验展示的便捷性，目前暂定支持以下常见虚拟环境的检测：**
>
> - VMware
> - Docker

### 1 VMware支持

#### 1.1 Windows虚拟环境检测

##### 1.1.1 检测VMWare特定服务

**检测指标如下：**

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



#### 1.2 Linux虚拟环境检测



### 2 Docker支持



## DebugEnv-Detector模块



## 输出标准定义

### 1 日志输出标准

- 日志格式

```bash
# 调试信息，用于调试功能模块，程序release后需关闭
[DEBUG][TIMESTAMP] DEBUG_MSG

# 提示信息，用于打印给用户提示信息
[INFO][TIMESTAMP] INFO_MSG

# 告警信息，用于告警用户诸如环境配置不当等问题
[WARN][TIMESTAMP] WARN_MSG

# 报错信息，用于提示用户程序错误
[ERROR][TIMESTAMP] ERROR_MSG

# 其中TIMESTAMP为时间戳，精确到秒
HH:MM:SS
```

- 日志输出方式：
	- 输出到stdout
	- 输出到run.log

### 2 检测结果输出标准

#### 2.1 命令行输出

- 将检测项目及其结果输出到stdout
- 输出格式如下：

```bash
# 没想好，可以参考https://github.com/LordNoteworthy/al-khaser的输出格式
```

#### 2.2 图形化输出（optional，视时间人手情况而定）

- 在拥有桌面系统的环境里，可以图形化输出检测结果
- 输出格式待定
	- 如Windows下弹出一个message box
	- 如Linux下采用zenity或notify-send或whiptail或dialog实现弹窗（美观性由高到低）