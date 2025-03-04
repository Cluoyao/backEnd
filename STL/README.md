# STL
> * [vector如何扩展内存和释放内存](#vector如何扩展内存和释放内存)


## [vector如何扩展内存和释放内存](https://www.cnblogs.com/biyeymyhjob/archive/2012/09/12/2674004.html)
> * 内存增长
> * [1.5还是2倍扩容](https://blog.csdn.net/dengheCSDN/article/details/78985684)
> * 内存释放

## 各种容器对比

|容器	|底层数据结构|时间复杂度	|有无序	|可不可重复 |	其他|
| :------: | :------: | :------: |:------: |:------: |:------: |
|array		|数组			|	随机读改 O(1)| 无序	|可重复	|支持快速随机访问                                                         |                               
|vector		|数组			|	随机读改、尾部插入、尾部删除 O(1)、头部插入、头部删除 O(n)	|无序	|可重复	|支持快速随机访问                                                         |
|list		|双向链表			|插入、删除 O(1)、随机读改 O(n)								|无序	|可重复	|支持快速增删                                                            |  
|deque		|双端队列			|头尾插入、头尾删除 O(1)									|无序	|可重复	|一个中央控制器 + 多个缓冲区，支持首尾快速增删，支持随机访问                    |           
|stack		|deque / list	|顶部插入、顶部删除 O(1)									|无序	|可重复	|deque 或 list 封闭头端开口，不用 vector 的原因应该是容量大小有限制，扩容耗时   | 
|queue		|deque / list	|尾部插入、头部删除 O(1)									|无序	|可重复	|deque 或 list 封闭头端开口，不用 vector 的原因应该是容量大小有限制，扩容耗时  	|
|priority_queue			|vector + max - heap	|插入、删除 O(log2n)				|有序	|可重复	|vector容器 + heap处理规则                                               |   
|set		|红黑树			|插入、删除、查找 O(log2n)									|有序	|不可重复																		|
|multiset	|红黑树			|插入、删除、查找 O(log2n)									|有序	|可重复  |																		 |
|map		|红黑树			|插入、删除、查找 O(log2n)									|有序	|不可重复|																		|
|multimap	|红黑树			|插入、删除、查找 O(log2n)									|有序	|可重复  |	 |

