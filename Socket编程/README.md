# 网络编程

> * [网络编程的步骤](#网络编程的步骤)
> * [常用API](#常用API)
> * [TCP中的accept和connect和listen的关系](#TCP中的accept和connect和listen的关系)
> * [UDP中的connect](#UDP中的connect)
> * [广播和组播过程](#广播和组播过程)
> * [大量TIMEOUT或CLOSEWAIT状态](#大量TIMEOUT或CLOSEWAIT状态)
> * [实现优雅关闭](#实现优雅关闭)
> * [解决TCP粘包](#解决TCP粘包)
> * [select可以判断网络断开吗](#select可以判断网络断开吗)
> * [send和read的阻塞和非阻塞情况](#send和read的阻塞和非阻塞情况)
> * [网络字节序和主机序](#网络字节序和主机序)
> * [IP地址分类及转换](#IP地址分类及转换)
> * [select实现异步connect](#select实现异步connect)
> * [为什么忽略SIGPIPE信号](#为什么忽略SIGPIPE信号)
> * [如何设置非阻塞](#如何设置非阻塞)



## 网络编程步骤

### TCP

* 服务端：socket -> bind -> listen -> accept -> recv/send -> close
    * 创建一个socket，用函数socket()，设置SOCK_STREAM
    * 设置服务器地址和侦听端口，初始化要绑定的网络地址结构
    * 绑定服务器端IP地址、端口等信息到socket上，用函数bind()
    * 设置允许的最大连接数，用函数listen()
    * 接收客户端上来的连接，用函数accept()
    * 收发数据，用函数send()和recv()，或者read()和write()
    * 关闭网络连接close()，需要关闭服务端sock和accept产生的客户端sock文件描述符

* 客户端：socket -> connect -> send/recv -> close
    * 创建一个socket，用函数socket()
    * 设置要连接的对方的IP地址和端口等属性
    * 连接服务器，用函数connect()
    * 收发数据，用函数send()和recv()，或read()和write()
    * 关闭网络连接close()
* 注意
    * INADDR_ANY表示本机任意地址，一般服务器端都可以这样写
    * accept中接收的是客户端的地址，返回对应当前客户端的一个clisock文件描述符，表示当前客户端的tcp连接
    * send和recv中接收的是新建立的客户端的sock地址

### UDP
* 服务端：socket -> bind -> recvfrom/sendto -> close
    * 建立套接字文件描述符，使用函数socket()，设置SOCK_DGRAM
    * 设置服务器地址和侦听端口，初始化要绑定的网络地址结构
    * 绑定侦听端口，使用bind()函数，将套接字文件描述符和一个地址类型变量进行绑定
    * 接收客户端的数据，使用recvfrom()函数接收客户端的网络数据
    * 向客户端发送数据，使用sendto()函数向服务器主机发送数据
    * 关闭套接字，使用close()函数释放资源
* 客户端：socket -> sendto/recvfrom -> close
    * 建立套接字文件描述符，socket()
    * 设置服务器地址和端口，struct sockaddr
    * 向服务器发送数据，sendto()
    * 接收服务器的数据，recvfrom()
    * 关闭套接字，close()
* 注意
    * sendto和recvfrom的第56个参数是sock地址
        * 服务器端的recvfrom和sendto都是cli地址
        * 客户端sendto是服务器端的地址，recvfrom是新建的from地址
    * UDP不用listen，accept，因为UDP无连接
    * UDP通过sendto函数完成套接字的地址分配工作
        * 第一阶段：向UDP套接字注册IP和端口号
        * 第二阶段：传输数据
        * 第三阶段：删除UDP套接字中注册的目标地址信息
    * 每次调用sendto函数都重复上述过程，每次都变更地址，因此可以重复利用同一UDP套接字向不同的目标传输数据
    
## 常用API
sendto、recvfrom保存对端的地址

* sendto
* recvfrom

## [TCP中的accept和connect和listen的关系](https://blog.csdn.net/tennysonsky/article/details/45621341)

### listen
* listen功能
    * listen函数把一个未连接的套接字转换成一个被动套接字，指示内核应接受指向该套接字的连接请求
    * 参数 backlog 的作用是设置内核中连接队列的长度
    * 根据TCP状态转换图，调用listen导致套接字从CLOSED状态转换成LISTEN状态。 

* 是否阻塞
    * listen()函数不会阻塞，它将该套接字和套接字对应的连接队列长度告诉 Linux 内核，然后，listen()函数就结束。

* backlog的作用
    * backlog是队列的长度，内核为任何一个给定的监听套接口维护两个队列：
        * 未完成连接队列（incomplete connection queue），每个这样的 SYN 分节对应其中一项：已由某个客户发出并到达服务器，而服务器正在等待完成相应的 TCP 三次握手过程。这些套接口处于 SYN_RCVD 状态。
        * 已完成连接队列（completed connection queue），每个已完成 TCP 三次握手过程的客户对应其中一项。这些套接口处于 ESTABLISHED 状态
    * 当有一个客户端主动连接（connect()），Linux 内核就自动完成TCP 三次握手，该项就从未完成连接队列移到已完成连接队列的队尾，将建立好的链接自动存储到队列中，如此重复
    * backlog 参数历史上被定义为上面两个队列的大小之和，大多数实现默认值为 5
    
### connect
* connect功能
    * 对于客户端的 connect() 函数，该函数的功能为客户端主动连接服务器，建立连接是通过三次握手，而这个连接的过程是由内核完成，不是这个函数完成的，这个函数的作用仅仅是通知 Linux 内核，让 Linux 内核自动完成 TCP 三次握手连接最后把连接的结果返回给这个函数的返回值（成功连接为0， 失败为-1）。
    * connect之后是三次握手

* 是否阻塞
    * 通常的情况，客户端的connect() 函数默认会一直阻塞，直到三次握手成功或超时失败才返回（正常的情况，这个过程很快完成）。


### accept
* accept功能
    * accept()函数功能是，从处于 established 状态的连接队列头部取出一个已经完成的连接(**三次握手之后**)

* 是否阻塞
    * 如果这个队列没有已经完成的连接，accept()函数就会阻塞，直到取出队列中已完成的用户连接为止。 
    * 如果，服务器不能及时调用 accept() 取走队列中已完成的连接，队列满掉后会怎样呢？UNP（《unix网络编程》）告诉我们，服务器的连接队列满掉后，服务器不会对再对建立新连接的syn进行应答，所以客户端的 connect 就会返回 ETIMEDOUT

## UDP中的connect
UDP的connect和TCP的connect完全不同，UDP不会引起三次握手

* 未连接的UDP传输数据
    * 第一阶段：向UDP套接字注册IP和端口号
    * 第二阶段：传输数据
    * 第三阶段：删除UDP套接字中注册的目标地址信息
* 已连接的UDP传输数据
    * 第一阶段：向UDP套接字注册IP和端口号
    * 第二阶段：传输数据
    * 第三阶段：传输数据

* 可以提高传输效率
* 采用connect的UDP发送接受报文可以调用send,write和recv,read操作，也可以调用sendto,recvfrom，此时需要将第五和第六个参数置为NULL或0
* 由已连接的UDP套接口引发的异步错误，返回给他们所在的进程。相反我们说过，未连接UDP套接口不接收任何异步错误给一个UDP套接口，connect后的udp套接口write可以检测发送数据成功与否，直接sendto无法检测

* 多次调用connect拥有一个已连接UDP套接口的进程的作用
    * 指定新的IP地址和端口号
    * 断开套接口
    

## 广播和组播过程

* 广播
	* 只适用于局域网
	* 只能向同一网络中的主机传输数据，
* 组播
	* 适用于局域网和广域网（internet） 

## 大量TIMEOUT或CLOSEWAIT状态

## 实现优雅关闭

## 解决TCP粘包
### 什么是TCP粘包

### 如何解决

## select可以直接判断网络断开吗
不可以。若网络断开，select检测描述符会发生读事件，这时调用read函数发现读到的数据长度为0.

## send和recv的阻塞和非阻塞情况
send函数返回100，并不是将100个字节的数据发送到网络上或对端，而是发送到了协议栈的写缓冲区，至于什么时候发送，由协议栈决定。

### send
* 阻塞
    * 一直等待，直到写缓冲区有空闲
        * 成功写返回发送数据长度
        * 失败返回-1
* 非阻塞
    * 不等待，立即返回，成功返回数据长度
    *  返回-1，判断错误码
        *  若错误码为EAGAIN或EWOULDBLOCK则表示写缓冲区不空闲
        *  若错误码为ERROR，则表示失败

### recv
* 阻塞
    * 一直等待，直到读缓冲区有数据
        * 成功写返回数据长度
        * 失败返回-1
* 非阻塞
    * 不等待，立即返回，成功返回数据长度
    *  返回-1，判断错误码
        *  若错误码为EAGAIN或EWOULDBLOCK则表示读缓冲区没数据
        *  若错误码为ERROR，则表示失败
    * 返回0
        * 对端关闭连接 

## 网络字节序和主机序
字节序分为大端字节序和小端字节序，大端字节序也称网络字节序，小端字节序也称为主机字节序。

* 大端字节序
    * 一个整数的高位字节存储在低位地址，低位字节存储在高位地址 
* 小端字节序
    *  高位字节存储在高位地址，低位字节存储在低位地址
* 转换API
    * htonl 主机序转网络序，长整型，用于转换IP地址
    * htons 主机序转网络序，短整型，用于转换端口号
    * ntohl 网络序转主机序
    * ntohs 网络序转主机序

## IP地址分类及转换
### IP地址分类

### IP转换
点分十进制转换成
 
## [select实现异步connect](https://blog.csdn.net/nphyez/article/details/10268723)
通常阻塞的connect 函数会等待三次握手成功或失败后返回，0成功，-1失败。如果对方未响应，要隔6s，重发尝试，可能要等待75s的尝试并最终返回超时，才得知连接失败。即使是一次尝试成功，也会等待几毫秒到几秒的时间，如果此期间有其他事务要处理，则会白白浪费时间，而用非阻塞的connect 则可以做到并行，提高效率。

### 实现步骤
* 创建socket，返回套接字描述符；
* 调用fcntl 把套接字描述符设置成**非阻塞**；
* 调用connect 开始建立连接；
* 判断连接是否成功建立。

### 判断连接是否成功建立：
* 如果为**非阻塞**模式，则调用connect()后函数立即返回，如果连接不能马上建立成功（返回-1），则errno设置为EINPROGRESS，此时TCP三次握手仍在继续。 
    * 如果connect 返回0，表示连接成功（服务器和客户端在同一台机器上时就有可能发生这种情况）
    * 失败可以调用select()检测非阻塞connect是否完成。select指定的超时时间可以比connect的超时时间短，因此可以防止连接线程长时间阻塞在connect处。
* 调用select 来等待连接建立成功完成；
    * 如果select 返回0，则表示建立连接超时。我们返回超时错误给用户，同时关闭连接，以防止三路握手操作继续进行下去。
    * 如果select 返回大于0的值，并不是成功建立连接，而是表示套接字描述符可读或可写
        *  当连接建立成功时，套接字描述符变成可写（连接建立时，写缓冲区空闲，所以可写）
        *  当连接建立出错时，套接字描述符变成既可读又可写（由于有未决的错误，从而可读又可写）
    * 如果套接口描述符可写，则我们可以通过调用getsockopt来得到套接口上待处理的错误（SO_ERROR）
        * 如果连接建立成功，这个错误值将是0
        * 如果建立连接时遇到错误，则这个值是连接错误所对应的errno值（比如：ECONNREFUSED,ETIMEDOUT等）。

## 为什么忽略SIGPIPE信号
往一个读端关闭的管道或者读端关闭的socket连接中写入数据，会引发SIGPIPE信号。当系统受到该信号会结束进程是，但我们不希望因为错误的写操作导致程序退出。

* 通过sigaction函数设置信号，将handler设置为SIG_IGN将其忽略
* 通过send函数的MSG_NOSIGNAL来禁止写操作触发SIGPIPE信号

## 如何设置文件描述符非阻塞
* 通过fcntl设置
```c++
int flag = fcntl(fd, F_GETFL);
flag |= O_NONBLOCK;
fctncl(fd, F_SETFL, flag);
```

