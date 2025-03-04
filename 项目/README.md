# 项目

## Linux下多并发web服务器
> * [简述](#简述)
> * [问题](#问题)

## 无人机图像拼接及车辆检测定位
> * [无人机简述](#无人机简述)
> * [无人机问题](#无人机问题)

## 设定场景下的车牌识别
> * [车牌简述](#车牌简述)
> * [车牌问题](#车牌问题)

## 2019华为软挑
> * [华为简述](#华为简述)
> * [华为问题](#华为问题)

## 简述
### 功能
* 这个项目是基于C/C++语言和B/S模型完成的web服务器，主要实现了web端的注册和登录、实现了同步和异步日志，记录服务器运行状态的功能，并支持http长连接，不支持管线化请求。

### 框架
* 项目框架主要分为I/O处理单元、逻辑处理单元和存储单元三个模块。其中I/O处理单元和逻辑处理单元对应**半同步/半反应堆线程池**，逻辑处理单元和存储单元对**应数据库连接池和日志系统**，这里实现了同步和异步写日志，另外还实现了非活动连接的处理和压力测试。

### 模块设计
#### **半同步/半反应堆连接池**
* 主线程为异步线程
    * 负责监听文件描述符，接收socket新连接，若当前监听的socket发生了读写事件，然后将任务插入到请求队列
* 请求队列
    * list链表实现，信号量sem提醒是否有任务要处理，线程池通过数组存储
* 工作线程
    * 创建固定数量的线程
        * 构造函数中创建线程池,pthread_create函数中将类的对象(this)作为参数传递给静态函数(worker),在静态函数中引用这个对象,并调用其动态方法(run)
        * pthread_create(m_threads+i, NULL, worker, this)
        * 设置线程分离detach，自动回收
    * 向请求队列中添加任务
        * 通过list容器创建请求队列
        * 向队列中添加时,通过互斥锁保证线程安全
        * 传进来的参数有读写操作的标志位，指示工作线程是读操作还是写操作
        * 添加完成后通过信号量提醒有任务要处理sem.post()
    * 工作静态函数
        * 调用对象,使用对象的动态方法run
    * 动态方法run执行任务
        * 信号量等待sem.wait()
        * 被唤醒后先加互斥锁
        * 从请求队列中取出第一个任务list.front(),将任务从请求队列删除list.pop_front()
        * process(模板类中的方法,这里是http类)进行处理
    

#### **主从状态机**
主线程负责读取客户端的请求，放到指定buffer内，（此时存在一个读取的read_index）由工作线程对http请求进行解析，这里参考游双的高性能服务器编程设计了主从状态机，其中主状态机从内部调用从状态机。

* 主状态机，负责解析请求行和头部具体数据
    * 两个状态CHECK_STATE_REQUESTLINE(请求行)，CHECK_STATE_HEADER(头部字段)，CHECK_STATE_CONTENT(当请求头部有很多字段时，判断是否读入完整)
    * 主状态机默认状态为解析http请求行，获得请求方法，目标文件url，http版本号，若能获得完整请求行，将状态转移为CHECK_STATE_HEADER
        * 请求行查找数据中空格+\t第一次出现的位置，判断请求方法是GET还是POST
        * 解析目标文件
        * 继续在剩余数据查找空格+\t第一次出现的位置，判断http版本是否是http/1.1
    * 解析http请求的一个头部信息，支持GET和POST请求，若为GET，根据空行判断是否解析完毕，若为POST，则设置cgi标志位，在响应代码中继续处理，若m_content_length不为0，状态转移为CHECK_STATE_CONTENT
        * 解析host，connection，content-length头部字段，更新m_host，m_linger，m_content_length
        * 解析connection时，判断是否是keep-alive长连接，设置长连接标志位
    * 判断请求头部是否完整读入，指主线程是否完整读入到buffer中
        * read_index >= m_content_length + m_checked_index 
* 从状态机，负责从buffer中读取完整的行数据
    * 存在自己的check_index 
    * 三个状态LINE_OK（读取成功），LINE_OPEN（未读取到完整请求），LINE_BAD（请求存在语法错误）
    * 初始状态LINK_OK，当buffer中有数据则，从状态机开始工作
    * 从buffer中解析出一行，通过判断是否存在行结束符'\r\n'，当buffer[check] == '\r'时
        * 如果存在buffer[check+1] == '\n'，那么读取到完整行，给主状态机分析
        * 如果不存在buffer[check+1] == '\r'，返回LINE_OPEN，表示还要继续读取
        * 否则，返回LINE_BAD
* http响应请求，若得到完整正确的http请求行，执行do_request()
    * get和post请求不同的方法处理
        * get方法的目标文件即之前主从状态机解析的url
        * post方法是先根据post最后一行提取出用户名和密码，进行注册和登录校验，根据post请求的不同标志位，跳转不同的文件
    * 将网站根目录和url文件拼接，然后通过stat判断该文件是否存在，是否可读，是否是目录，然后为了提高访问速度，将普通文件映射到内存地址
* 根据do_request的返回状态，工作线程向指定buffer写入响应请求
    * 先写状态行 http/1.1 状态码 状态消息
    * 写消息报头，包含content-length，connection状态，根据请求解析出的状态决定
    * 添加空行和响应正文
    * 向指定buffer写入响应请求，更新write_idx指示buffer中的长度
* process函数
根据传进来的参数，确定是读操作还是写操作 
    * read
        * 循环读取完请求报文，存储到指定的buffer，并设置read_index指示长度
    * write
        * 当process_write写完后，注册epollout事件，此时工作线程会通过判断write_index是否为空来判断是否能写 
        * 给客户端发送响应请求，判断是否发送成功，byte_to_send <= byte_have_send
        * 若发送成功，先取消mmap映射，然后判断是否是长连接，长连接重置http类实例，注册读事件，不关闭连接，否则关闭连接
        * 若发送不成功，判断是否是写缓冲区满了，若不是因为缓冲区满了而失败，取消mmap映射，关闭连接；若eagain则满了，注册写事件，等待下一次写事件触发（当写缓冲区从不可写变为可写，触发epollout），因此在此期间无法立即接收到同一用户的下一请求，但可以保证连接的完整性 

#### **数据库连接池**
* 懒汉单例模式创建数据库连接池
* 链表实现连接池，里面放的是数据库连接，线程池通过数组存储，list是请求队列
* 功能
    * 从连接池获取连接
    * 放回连接池，释放当前连接
    * 销毁连接池
* 设置信号量，首先设置为最大连接次数，每次取出连接，信号量原子减1，释放连接原子加1，若连接池内没有连接了，则阻塞等待

#### **注册和登录校验**
* CGI多进程
    * 在http响应的代码中，创建管道，通过fork创建子进程
    * 子进程中，关闭管道读端，将标准输出重定向到管道写端dup2，子进程通过execl执行登录注册程序，在程序中先初始化数据库连接池，然后获取连接，根据http的POST请求获取user和name进行注册和登录校验，返回成功或失败
    * 父进程中，关闭管道写端，打开管道读端，读取CGI的返回值，响应http的请求
* 同步线程
    * 由于CGI多进程中使用数据库连接池非常鸡肋，因为在execl中创建数据库连接池，将数据库连接池初始化放在主线程中，响应http的代码中从数据库连接池获取连接，执行注册和登录验证。

#### **服务器日志系统**
**请求队列，同步、异步，超行、按天分类的设计**

* 请求队列的设计
    * stl中的队列，线程不安全，每次使用push等操作时，需要频繁加锁
    * 通过条件变量和循环数组实现线程安全的队列
    * 条件变量的使用，需要注意pthread_cond_wait前需加锁，且wait后需要再次检测资源是否可用，可以用while

* 同步
    * 同步是调用写操作时，将输出信息写入日志文件
    * 当单条日志比较大的时候，同步模式会阻塞整个处理流程，那么应用能处理的并发能力将有所下降，尤其是在峰值的时候，写日志可能成为系统的瓶颈
    * 服务崩溃或者重启服务的时候不会造成日志丢失
* 异步
    * 创建一个写线程，工作线程将要写的内容push进请求队列，然后通过写线程写入文件 
    * 在峰值的时候能够平滑过渡，降低写日志的压力
    * 在服务崩溃或者重启服务的时候可能会造成部分日志丢失

注意：同步异步通**过初始化时是否制定队列大小**（表示在队列中可以放几条数据）来判断，若队列大小为0，则为同步，否则为异步

* 超行、按天分类
    * 服务器启动按当前时刻创建日志,前缀为时间,后缀为自定义log文件名
    * 记录创建日志的时间day和行数count
    * 日志写入前会判断当前day是否为创建日志的时间,行数是否超过最大行限制
    * 若为创建日志时间,写入日志,否则按当前时间创建新log,更新创建时间和行数
    * 若行数超过最大行限制,在当前日志的末尾加count/max_lines为后缀创建新log 
    
#### **非活动连接处理**
利用alarm函数周期性地触发SIGALRM信号,调用心跳函数，该信号的信号处理函数利用管道通知主循环执行定时器链表上的定时任务.

**定时器和心跳函数的设计，这里alarm设置为5秒，连接超时为15秒：**

* 定时器（主要涉及双向链表的插入，删除操作，添加定时器的事件复杂度是O(n),删除定时器的事件复杂度是O(1)，因为直接给了定时器结点，而不是值）
    * 双向链表,创建头尾节点,头尾节点没有意义，仅仅统一方便调整
    * add_timer函数将目标定时器添加到链表中，添加时按照升序添加
        * 若当前链表中只有头尾节点，直接插入
        * 否则，将定时器按升序插入
    * adjust_timer函数当定时任务发生变化,调整对应定时器在链表中的位置，如客户端在设定时间内有数据收发,则当前时刻对该定时器重新设定时间，这里只是往后延长超时时间
        * 如果被调整的目标定时器在尾部，或定时器新的超时值仍然小于下一个定时器的超时，不用调整
        * 否则先将定时器从链表取出，重新插入链表
    * del_timer函数将超时的定时器从链表中删除
        * 常规双向链表删除结点
* 回调函数
    * 将定时任务封装成回调函数，由用户自己决定，这里是删除非活动socket上的注册事件，并关闭
* 心跳函数
    * 遍历定时器链表从头结点开始依次处理每个定时器,直到遇到尚未到期的定时器
    * 若当前时间小于定时器设定时间,跳出循环,即未找到到期的定时器
    * 若当前时间大于定时器设定时间，即找到了到期的定时器,执行回调函数,然后将它从链表中删除，然后继续遍历

**定时器的使用**

* 创建管道
* 设置信号处理函数SIGALRM（时间到了触发）和SIGTERM（kill会触发）
    * 通过struct sigaction结构体和sigaction函数注册信号捕捉函数 
    * 在结构体的回调函数中设置从管道读端写入信号的名字
* 利用I/O复用系统监听管道读端文件描述符的可读事件
* 信息值传递给主循环，主循环再根据接收到的信号值执行目标信号对应的逻辑代码

#### **压力测试**
*  思想
    *  通过单纯的I/O复用方式对服务器施压，因为线程和进程的调度本身也要占用一定CPU时间
    *  如果服务器足够稳定，服务器和测试程序会不断的交换数据
* 设计
    *  循环创建num个socket，向服务器发起num个TCP连接
    *  每个连接connect成功后，通过在epoll内核事件表注册epollin、epollout，监听读写事件
    *  向服务器端写request数据GET http://localhost/index.html HTTP/1.1 \r\n Connetcion:keep-alive\r\n\r\n，必须满足http中get请求的报文格式
    *  每个连接不停的向服务器发送请求request，访问主机上的文件
    *  从服务器端读数据，直接打印在终端上，显示读到的数据和字节数，与主机交换数据

### 以一个请求到来具体的处理过程介绍项目工作流程
* web端和服务器端建立连接，采用**epoll的边缘触发模式**同时监听多个文件描述符，采用**同步I/O模拟proactor模式**处理事件，此时主线程负责监听客户端是否发起请求，当web端发起http请求时，主线程接收请求报文，然后将任务插入请求队列，由工作线程通过竞争从请求队列中获取任务，通过**http类中的主从状态机**对请求报文进行分析，根据请求报文对客户端进行http响应，然后由主线程给客户端发送响应报文。
* 单例模式创建数据库连接池，避免频繁建立连接，用于后续web端登录和注册校验访问服务器数据库
* 访问服务器端的数据库实现web端的登录和注册，web访问的欢迎界面为GET请求，登录和注册界面是POST请求。欢迎界面有新用户（0）和已有账号（1）两个选项，若选择新用户，会跳转注册(3)界面，注册成功或选择已有账号，跳转登录(2)界面，注册或登录失败会提示失败，成功和失败为0，1
* 服务器运行过程中可以通过同步和异步的方式写入日志文件，记录服务器运行状态
    * 同步的方式下，工作线程直接写入日志文件
    * 异步的话会另外创建一个写线程，工作线程将要写的内容push进请求队列，然后通过写线程写入文件
    * 另外，日志文件支持按日期分类，和超过最大行数自动创建新文件
* 非活动连接的处理。由于非活跃连接占用了连接资源，严重影响服务器的性能，通过实现一个服务器定时器，处理这种非活跃连接，释放连接资源。利用alarm函数周期性地触发SIGALRM信号,该信号的信号处理函数利用管道通知主循环执行定时器链表上的定时任务.

## 问题
### reactor模式/proactor模式
* reactor模式中，主线程只负责监听文件描述符上是否有事件发生，有的话立即通知工作线程，读写数据、接受新连接及处理客户请求均在工作线程中完成。
* proactor模式中，主线程和内核负责处理读写数据、接受新连接等I/O操作，工作线程仅负责业务逻辑，如处理客户请求

### 同步I/O模拟proactor模式
* 主线程执行数据读写操作，读写完成之后，主线程通知工作线程。从工作线程的角度来看，他们直接获得了数据读写的结果，接下来只负责逻辑处理。

### 五种I/O
> * 阻塞IO:调用者调用了某个函数，等待这个函数返回，期间什么也不做，不停的去检查这个函数有没有返回，必须等这个函数返回才能进行下一步动作
> * 非阻塞IO:非阻塞等待，每隔一段时间就去检测IO事件是否就绪。没有就绪就可以做其他事。非阻塞I/O执行系统调用总是立即返回，不管时间是否已经发生，若时间没有发生，则返回-1，此时可以根据errno区分这两种情况，对于accept，recv和send，事件未发生时，errno通常被设置成eagain
> * 信号驱动IO:linux用套接口进行信号驱动IO，安装一个信号处理函数，进程继续运行并不阻塞，当IO时间就绪，进程收到SIGIO信号。然后处理IO事件。
> * IO复用:linux用select/poll函数实现IO复用模型，这两个函数也会使进程阻塞，但是和阻塞IO所不同的是这两个函数可以同时阻塞多个IO操作。而且可以同时对多个读操作、写操作的IO函数进行检测。知道有数据可读或可写时，才真正调用IO操作函数
> * 异步IO:linux中，可以调用aio_read函数告诉内核描述字缓冲区指针和缓冲区的大小、文件偏移及通知的方式，然后立即返回，当内核将数据拷贝到缓冲区后，再通知应用程序。

**注意：阻塞I/O，非阻塞I/O，信号驱动I/O和I/O复用都是同步I/O。同步I/O要求用户代码自行执行I/O操作，异步I/O机制则由内核完成I/O操作**

### [select/poll/epoll](https://blog.csdn.net/u012398613/article/details/51787548)
* 调用函数
    * select和poll都是一个函数，epoll是一组函数
* 文件描述符数量
    * select通过线性表描述文件描述符集合，文件描述符有上限，一般是1024，但可以修改源码，重新编译内核，不推荐
    * poll是链表描述，突破了文件描述符上限，最大可以打开文件的数目
    * epoll通过红黑树描述，最大可以打开文件的数目，可以通过命令ulimit -n number修改，仅对当前终端有效
* 将文件描述符从用户传给内核
    * select和poll通过将所有文件描述符拷贝到内核态，每次调用都需要拷贝
    * epoll通过epoll_create建立一棵红黑树，通过epoll_ctl将要监听的文件描述符注册到红黑树上
* 内核判断就绪的文件描述符
    * select和poll通过遍历文件描述符集合，判断哪个文件描述符上有事件发生
    * epoll
        * epoll_create时，内核除了帮我们在epoll文件系统里建了个红黑树用于存储以后epoll_ctl传来的fd外，还会再建立一个list链表，用于存储准备就绪的事件，当epoll_wait调用时，仅仅观察这个list链表里有没有数据即可。
        * epoll是根据每个fd上面的回调函数(中断函数)判断，只有发生了事件的socket才会主动的去调用 callback函数，其他空闲状态socket则不会，若是就绪事件，插入list
    
* 应用程序索引就绪文件描述符
    *  select/poll只返回发生了事件的文件描述符的个数，若知道是哪个发生了事件，同样需要遍历
    *  epoll返回的发生了事件的个数和结构体数组，结构体包含socket的信息，因此直接处理返回的数组即可
* 工作模式
    * select和poll都只能工作在相对低效的LT模式下
    * epoll则可以工作在ET高效模式，并且epoll还支持EPOLLONESHOT事件，该事件能进一步减少可读、可写和异常事件被触发的次数。   

### epoll的ET、LT、epolloneshot
* LT水平触发模式
    * epoll_wait检测到文件描述符有事件发生，则将其通知给应用程序，应用程序可以不立即处理该事件。
    * 当下一次调用epoll_wait时，epoll_wait还会再次向应用程序报告此事件，直至被处理
* ET边缘触发模式
    *  epoll_wait检测到文件描述符有事件发生，则将其通知给应用程序，应用程序必须立即处理该事件
    * 必须要一次性将数据读取完，使用非阻塞I/O，读取到出现eagain
* EPOLLONESHOT
    * 一个线程读取某个socket上的数据后开始处理数据，在处理过程中该socket上又有新数据可读，此时另一个线程被唤醒读取，此时出现两个线程处理同一个socket
    * 我们期望的是一个socket连接在任一时刻都只被一个线程处理，通过epoll_ctl对该文件描述符注册epolloneshot事件，注册时候，一个线程处理socket时，其他线程将无法处理，当该线程处理完后，需要通过epoll_ctl重置

### 什么是优雅关闭连接
close()和shutdown()

### [多线程的pthread_cond_wait后，记住再次检测条件](https://tower.iteye.com/blog/309294)

### 统一事件源
信号是一种异步事件:信号处理函数和程序的主循环是两条不同的执行路线。一种典型的解决方案是：把信号的主要处理逻辑放到程序的主循环中，当信号处理函数被触发时，它只是简单地通知主循环程序接收到信号，并把信息值传递给主循环，主循环再根据接收到的信号值执行目标信号对应的逻辑代码。

* 信号处理函数通常使用管道来将信号传递给主循环：
    * 信号处理函数往管道的写端写入信号值类型
    * 利用I/O复用系统监听管道读端文件描述符的可读事件
    * 主循环则从管道的读端读出该信号值的类型

## 无人机简述
### 功能
对无人机回传图像进行拼接，通过融合回传的GPS数据实现定位，对拼接后的全景图进行车辆检测并定位，其中车辆检测是基于目标检测框架YOLO的二次开发。

### 框架
主要有回传图像和GPS的接收、解析，图像拼接及全景图车辆检测定位三部分。
我主要负责GPS的接收和解析以及全景图的车辆检测。

### 模块设计
#### 图像的接收和解析
54所负责
#### GPS的接收和解析
地面站以UDP组播的方式向客户端发送数据，每个GPS数据定义了数据结构，包括高度，经纬度，时间等信息，对结构体进行字节对齐，然后获取GPS帧的大小为128。

* 多播的流程
    * 创建socket，初始化socket地址信息和端口信息，绑定端口，加入到组播地址，添加组播权限
* GPS接收
    * 创建128字节的buffer， 通过recv进行接收，将buffer转型为GPS结构体的指针
* 解析
    * 从buffer中通过访问结构体成员，提取数据，**将GPS坐标转换为UTM坐标等**，并设置标志变量，存入txt文件.

#### 图像和GPS两者不同步，当时的解决办法
图像的传输频率慢，GPS的传输频率快，经过讨论确定了要使用的两种载荷，图像均是25HZ，GPS是25和30HZ，最终选择在界面开放两个接口，输入两个帧率，30帧时，每隔5帧扔掉一帧。

#### 利用fifo模拟GPS和图像接收的同步
通过UDP协议模拟图像端和飞控端收发数据，设置图像端传输速率为两秒一帧，飞控端传输速率为一秒一帧，以图像端接收到数据的时间戳为准，飞控端接收当前时刻的飞控数据，基于FIFO实现两者的数据同步。

* 实现图像的发送与接收
    * 发送
        * 定义图像数据包的格式
        ```C++
        struct Package
        {
        	int length;         //本次发送的数据长度
        	char data[1024];    //本次发送的数据buffer
        	int fin;            //标记是否是最后一个数据包
        }picture;

        ```
        * fopen将图像文件以可读可写的方式按二进制文件格式打开rb+
        * fseek先将文件指针定位到文件尾部，ftell得到该文件的总长度
        * fseek重新将文件指针定位到文件头
        * 以剩余长度是否大于0来判断是否发送完成，每次成功发送，将总长度-1024
        * 当剩余的长度<1024，则将该数据包标记为最后一个数据包
    * 接收
        * 通过判断是否是最后一个包来判断是否要继续接收
        * 第一次接收数据包后，以可读可写的方式打开一个二进制文件wb+(若没有，则创建)，后缀名为.jpg，以当前时间创建文件名
        * 每次接收数据包向打开的文件中写入数据
* 实现文本数据的发送与接收
    * 发送
        * 定义GPS数据包的长度为8个字节，可以一次性发完
        * 以可读的方式打开txt文件
        * 每次fread发完8个字节后，清空buffer，继续发送
    * 接收
        * 向二进制文件末添加数据，允许读；
        * 循环接收，每次接收完，判断管道读到的数据是否 > 0，否则清空buffer
        * 否则跳出，写入txt
* 实现两者的接收同步
    *  fifo的两端，图像端写，飞控端读
    *  飞控端的recv函数在while循环中一直读，但不写入txt
    *  图像端读到完整图像，此时操作fifo写端
    *  当飞控端接收到图像端发出的fifo信息，跳出循环，接收txt，写入txt
    *  注意此时的文件描述符是非阻塞的

#### 图像拼接与定位
* 单航带拼接
主要采用了Opencv中的Surf特征点检测器和Brisk特征点描述子

* **将第一帧图像直接放在我们的黑色模板中间，检测它的关键点和描述向量，对于第二帧，也是同样的操作**
    * 读取第一帧图像，并进行灰度转换，并提取关键点和对应的特征描述子。
    * 把第一帧图像直接放到拼接全景图的中间位置(2*img.cols,2*img.rows,黑色的背景。这将是我们的起始帧)
    * 读取新的视频帧，并进行灰度化。(动态调整需要求解的关键点的个数，让特征比较单一的图像求取的关键点多，防止因为特征单一而拼接不上图像，特征比较复杂的图像，求取的关键点少，从而缩短拼接时间。)
* **为了将第二帧图像跟第一帧拼接起来，我们先利用匹配器对关键点进行匹配，筛选出最佳的匹配**
    * 使用SURF检测器进行关键点检测，该检测器是sift的升级版，运行时间比它快，比其他检测器得到的关键点具有更好的鲁棒性。
    * 利用brisk描述子计算上一步关键点对应的特征描述符(特征向量)。
    * 基于上一帧和当前帧的关键点和描述子，我们利用暴力匹配器对关键点进行匹配，得到最佳匹配结果(即如果计算上一帧A点的匹配点对应当前帧的B点，那么反过来，我当前帧的B计算得到的匹配点是A点，我们才能认为他们是最佳匹配)，但就算是这样，我们的结果也是有无匹配的。
* **利用随机抽样一致性算法丢弃误匹配点对，最后我们选了50对匹配点，用来求仿射变换矩阵，利用该矩阵，将第二帧与第一帧配准，同时我们还会累乘得到的仿射变换矩阵，就是为了将第n帧与第1帧的位置关联起来。**
    * 去除误匹配点对，利用随机抽样一致性算法(RANSAC)丢弃误匹配点对，然后以最小匹配距离的3倍为阈值，将小于该阈值的匹配点对作为最佳匹配点对，并且只取前50对，以此来保证匹配点对的鲁棒性。
    * 利用前面求得的最佳匹配点对，计算前一帧图像和当前帧图像之间的仿射变换矩阵(3*2的单应性矩阵)，为了对齐当前帧和前n帧图像拼接的结果，我们需要将单应性矩阵进行累乘，这样才能将当前帧和前n帧图像的位置联系起来。(但这样会存在一个问题，如果单应性矩阵有问题，误差就会累积)
* **累乘又会导致误差传递，我们用了两种方法来筛掉可能的误差：一是计算上一个单应性矩阵与当前的单应性矩阵之差的二范数，设置阈值判断现在的单应性矩阵正不正常；二是前一个矩阵和当前矩阵的相关系数，相关系数太小就认为该矩阵不合理。**
    * 为了解决上一步可能存在的误差传递问题，利用视频图像的先验知识，因为视频是连续的，所以得到的单应性矩阵不可能发生剧烈的变化，也就是说，如果变化剧烈，那么这一帧图像就有问题。我们同时采用了两种方法来解决：一是计算前一帧和当前帧的单应性矩阵之差，求二范数，然后利用阈值对其进行比较。而是计算将上一帧的单应性矩阵看做原图，当前帧的单应性矩阵看做模板，进行模板匹配，得到相关系数，如果相关系数太小(<0.8)则认为该单应性矩阵不合理。正常是越接近1越好。
    * 利用上述方法得到的单应性矩阵，将图片贴合到全景图中，由于直接利用透视变换公式贴合，对高分辨率图像计算时间消耗大，我们采用模板贴图的方式，通过自己制作模板，将当前帧图像对齐到相应位置。通过一个3*2透视投影单应性矩阵累乘得到第(n-1)帧与第一帧之间的仿射变换矩阵，最后将视频拼接成一幅完整的大图
    * 保存当前帧的相关信息，为下一帧做准备，循环进行拼接。

* 多航带拼接
    基于无人机航迹约束的多航带图像矫正方法，该方法要求无人机飞行轨迹必须为方波。
    * 将第一条航带拼接后的所有图像中心点通过线性回归得到一条直线，得到该直线斜率，
    * 在第二条航带拼接时取前十帧像素点求取平均值得到一个确定点，从而确定第二条航带的拼接约束轨迹。

* 定位
   对图像完成拼接后，接下来对图像任意点进行GPS定位。传统的方法多为根据飞行高度，视场角，分辨率等参数求得比例尺，再根据图像中心点GPS递推出该帧图像其他各个点的GPS，但这种方法鲁棒性较差，需要对每帧非垂直下视图像进行矫正，会引入计算误差并累计。**为了消除这种误差累计，我们采用的是基于全局GPS的定位方法。**
    * 在拼接的过程中存储拼接图像中心像素点与对应的GPS点，形成拼接像素点集与UTM点集。
    * 将所有拼接图像的GPS集与对应的像素点集通过带有约束的仿射变换进行求解映射矩阵，但是存在可能无解形成空矩阵的情况，为此，采取的解决方法是通过逐渐减少GPS与对应像素点的总量，之后等间隔取点，如此反复循环直到求出为止，此方法可以在最大限度上保证精度。
    * 求取GPS与像素点的映射关系时，也可采用求取透视变换单应性矩阵的方法，这种方法不会出现空矩阵的情况，减少可求取次数。另外通过RANSAC算法来筛选特征，可以求出最优变换矩阵。


#### 全景图车辆检测
基于YOLO的二次开发，通过图像的分割与检测结果的融合，实现高分辨率图像的车辆检测

* 训练网络
    * 数据预处理
        * VEDAI原始数据标签中包含了目标四个角点坐标，是否遮挡等信息，经过OpenCV库处理后得到目标的BoundingRect信息，并筛掉飞机、船舶等其他目标。
        * 通过Python脚本将图片及标签生成VOC数据格式
        * 对现有数据进行数据增强，增加数据样本
    * 模型训练
        * 修改yolo-voc.config中的网络参数，训练图片大小，迭代次数，批数量等超参数后，开始训练模型，大概迭代40000次后，loss为2e-2，此时停止训练得到检测模型。

* 读入全景图，滑窗分割
    * 训练的模型输入图片size为512*512，因此采用滑窗算法对高分辨率图像进行裁剪存入文件夹，进行****_****特殊命名；
    * 启动程序，先将文件夹清空，然后将分割后的图片存入文件夹
     * 0000_0000：表示裁剪的第一行第一列图片，依次递增 
* 信息融合
    * 修改命令行测试为文件夹测试，每次测试一个文件夹，将1中裁剪后的图片文件夹送入yolov3进行检测，利用命名规则对检测后的各部分图片进行融合；
    * 将裁剪后的图像检测的四个信息（左上x,y和w,h）根据图片的命名规则映射到全景图中，将所有图片的信息全部存入txt，然后再从txt中读取，标记在全景图中

## 无人机问题

* 拼接过程中图像会越来越大，mask会自适应扩大以适应拼接贴图
    *  若贴图发现超出了当前的mask边界，则以超出的绝对值长度进行扩展当前的mask
    *  当前mask相较于变换前的mask可以推导出一个变换矩阵，将其累乘到放射变换矩阵中，则可以将新图准确的拼接在调整后的mask上
* 单航带拼接交叉或拼接变形问题
    * 由于无人机的前后左右摇晃，由于视场角不变，机体摇晃后获得的实际范围会不一致
    * 若无人机向前飞行，机尾一直向下偏，则会导致采集的图像前端范围 > 后端范围
    * 在拼接时，后一张图相对于前一张图得出仿射变换矩阵，由于对应位置的范围变窄，且仿射变换是相对于整幅图的变换，图像会越来越小
    * 通过修改H自由度矩阵，原来的是计算出来的缩放系数，这里将其设置为1，强制其不进行缩放，因此拼接出来的图像会有左右的移动，基本不影响拼接效果。
* 函数
    * estimateRigidTransform()：计算多个二维点对或者图像之间的最优仿射变换矩阵 （2行x3列），H可以是部分自由度，比如各向一致的切变。
    * getAffineTransform()：计算3个二维点对之间的仿射变换矩阵H（2行x3列），自由度为6.
    * warpAffine()：对输入图像进行仿射变换
    * findHomography： 计算多个二维点对之间的最优单映射变换矩阵 H（3行x3列） ，使用最小均方误差或者RANSAC方法 。
    * getPerspectiveTransform()：计算4个二维点对之间的透射变换矩阵 H（3行x3列）
    * warpPerspective()： 对输入图像进行透射变换
    * perspectiveTransform()：对二维或者三维矢量进行透射变换，也就是对输入二维坐标点或者三维坐标点进行投射变换。

## 华为简述
基于给定的道路、路口和车辆信息，规划车辆路径和出发时间，使系统调度时间最短。

### 给定数据
* 道路信息
    * (id,roadId,roadId,roadId,roadId) 
* 路口信息
    * (id,length,speed,channel,from,to,isDuplex) 
* 车辆信息
    * (id,from,to,speed,planTime) 

### 解决方案
根据所给信息，将问题抽象成求图中两点的最短路径。方案主要分三个部分，分别为发车策略、自定义路权函数和伪动态更新路权

* 发车策略
    * 对角线发车，只有两组。将所有车辆按行驶路线分为四组，右下，左上，右上，左下
    * 调整两组车辆的发车间隔。人为调整参数
    * 优先速度快的发车。将每组车辆按车辆行驶速度排序，速度大的车辆优先发车，按速度依次发车，时间间隔依次加50时间片
* 自定义路权函数
     * 图中两点间的路径，没有简单的考虑长度，而是综合考虑了各种因素
* 伪动态更新路权
    *  使相同起止点的车辆选择尽量选择不同的道路
    *  根据发车使用的道路次数，调整道路阻抗

### 模块设计
* 调整道路顺序
    * 根据路口给出的第一行信息，人为顺时针规定为上、下、左、右，这里标记0,1,2,3
    * 通过道路和路口的关系，找到0，2相互连接，1，3相互连接
    * 若当前序号为i，则当前道路是相连对应路口的(i+2)%4
    * 从第一行路口的“上（0）”开始顺时针遍历，也就是对应路口的2
    * 递归调整剩余道路的顺序和第一行相同，标记为上、下、左、右
* 分组
    * 先以道路长度为图的权重构建图，然后根据起止点求出每辆车的最短路径
    * 计算每辆车的最短路径中的每条路，相对于每个路口而言上下左右的数量
    * 若右>左，上>下，则分组在右下，其余以此类推，这样就将所有车分为四组
    * 对角线发车，右下和左上为一组，右上和左下为一组，发车不容易死锁
* 自定义路权函数
    * 创建有向图，其中路权函数考虑了长度length、车道数lines、道路限速speed、单双向道twoWay，**道路使用情况numOfUSe**
    * 长度length、车道数lines、道路限速speed路权元素需要归一化，其中长度元素占比相较于其他要高
    * 测试发现，车辆死锁极易发生在单向道及其附近，其他元素相同的情况下，将单向道的权重设置比双向道高，优先走双向道
    * 当道路使用次数较多时，则默认道路阻抗较大，通过调整参数尽量少选择该道路 
* 伪动态更新路权
    *  循环所有路口，以路口的最大发车数目为限制条件，每次路口只发一辆车
    *  记录更新道路使用次数，进而更新路权函数
* 最终路径
    * 当伪动态更新路权后，所有车辆都被考虑在内，此时根据路权函数，重新用dijkstra计算每辆车的路径
    * 根据发车策略，确定发车时间

## 华为问题
### 解释Dijkstra算法

### 了解A*寻路算法吗































