# 网络编程

> * [网络编程的步骤](#网络编程的步骤)
> * [广播和组播](#广播和组播)

## 网络编程步骤

* TCP：

	* 服务端：socket -> bind -> listen -> accept -> recv/send -> close。

	* 客户端：socket -> connect -> send/recv -> close。

* UDP：

	* 服务端：socket -> bind -> recvfrom/sendto -> close。

	* 客户端：socket -> sendto/recvfrom -> close。

## 广播和组播

* 广播
	* 只适用于局域网
* 组播
	* 适用于局域网和广域网（internet）  