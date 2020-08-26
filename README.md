# Simple Socket

## 特点

新版arm库，解决之前出现的3个问题。

1. 下载速度慢

采用双工异步IO通讯，局域网测试下载人员图片和模板速度可以达到100M/S。

2. 扩展性差

不关心具体业务数据，协议由用户定制，建议JSON格式通讯，类似HTTP通讯模型，扩展容易。

3. 兼容性问题

旧版协议很难兼容，新版协议向下兼容，服务器和客户端不存在兼容性问题。

4. 接口简洁易维护

接口函数很少，和websocket接口高度一致，通讯方式也和websocket类似。

5. 第三方库依赖少

源代码方式使用POCO网络库，只依赖GLOG,测试依赖GTEST，核心代码不依赖其他库。

6. 标准C接口

可以支持其他的任何语言调用，丰富的示例支持。

## 下一步

1. 高并发没有实战项目应用，需要测试高并发环境下面的使用。

2. 加入内存池和线程池的支持，优化数据buffer的内存管理和事件回调的效率。


## 平台

适用于多个平台:

*   Linux
*   Windows
*   Cygwin
*   MinGW
*   Android


## 发布页面

http://wiki.hongshi-tech.com/pages/viewpage.action?pageId=22579152

## 实战项目

1. D20e平台集成

服务器是WINDOWS或者LINUX的web服务器，Qt开发或者Java语言开发，管理多台D20e门禁设备
客户端是多台D20e设备，平台是ARM，Qt开发

2. 农商行

git代码页面： http://git.hongshi-tech.com/projects/HSNSH/repos/hs_d35_sdk/browse?at=refs%2Fheads%2Fdevelop
服务器是YP的平台服务器，C#语言开发，管理多台D35门禁机
客户端是多台D35设备，平台是ANDROID，Java语言开发
里面包括完整的C#二次封装和使用示例，ANDROID使用示例，测试用例