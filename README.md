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

2. 农商行
	git代码页面： http://git.hongshi-tech.com/projects/HSNSH/repos/hs_d35_sdk/browse?at=refs%2Fheads%2Fdevelop