# C++补充

> * [构造函数可以抛出异常吗，有什么问题？](#构造函数可以抛出异常吗有什么问题)
> * [初始化列表的异常怎么捕获？](#初始化列表的异常怎么捕获)
> * [析构函数可以抛出异常吗，有什么问题？](#析构函数可以抛出异常吗有什么问题)
> * [析构函数如何处理异常](#析构函数如何处理异常)
> * [智能指针](#智能指针)
> * [强制转换](#强制转换)
> * [RTTI](#RTTI)


## [构造函数可以抛出异常吗，有什么问题？](https://www.cnblogs.com/qinguoyi/p/10304882.html)
构造函数中应该避免抛出异常。
> * 构造函数中抛出异常后，对象的析构函数将不会被执行
> * 构造函数抛出异常时，本应该在析构函数中被delete的对象没有被delete，会导致内存泄露
> * 当对象发生部分构造时，已经构造完毕的子对象（非动态分配）将会逆序地被析构。

## 初始化列表的异常怎么捕获？
> * 初始化列表构造，当初始化列表出现异常时，程序还未进入函数体，因此函数体中的try-catch不能执行，catch也无法处理异常。可以通过函数try块解决该问题。
> * 函数try块中的try出现在表示构造函数初始值列表的冒号以及表示构造函数体的花括号之前，与这个try关联的catch既能处理构造函数体抛出的异常，也能处理成员初始化列表抛出的异常。


## 析构函数可以抛出异常吗，有什么问题？
析构函数不应该抛出异常
> * **其他正常，仅析构函数异常**。 如果析构函数抛出异常，则异常点之后的程序不会执行，如果析构函数在异常点之后执行了某些必要的动作比如释放某些资源，则这些动作不会执行，会造成诸如资源泄漏的问题。
> * **其他异常，且析构函数异常**。 通常异常发生时，c++的机制会调用已经构造对象的析构函数来释放资源，此时若析构函数本身也抛出异常，则前一个异常尚未处理，又有新的异常，会造成程序崩溃的问题。

## 析构函数如何处理异常？
> * 若析构函数抛出异常，调用std::abort()来终止程序
> * 在析构函数中catch捕获异常并作处理，吞下异常；
> * 如果客户需要对某个操作函数运次期间抛出的异常做出反应，class应该提供普通函数执行该操作，而非在析构函数中。

## [智能指针](https://www.cnblogs.com/TianFang/archive/2008/09/20/1294590.html)
智能指针有shared_ptr,weak_ptr,unique_ptr

* share_ptr核心要理解引用计数，什么时候销毁底层指针，还有赋值，拷贝构造时候的引用计数的变化，析构的时候要判断底层指针的引用计数为0了才能真正释放底层指针的内存

* shared_ptr是可以共享所有权的智能指针
    * shared_ptr的管理机制其实并不复杂，就是对所管理的对象进行了引用计数，当新增一个shared_ptr对该对象进行管理时，就将该对象的引用计数加一；减少一个shared_ptr对该对象进行管理时，就将该对象的引用计数减一，如果该对象的引用计数为0的时候，说明没有任何指针对其管理，才调用delete释放其所占的内存。   
    * 对shared_ptr进行初始化时不能将一个普通指针直接赋值给智能指针，因为一个是指针，一个是类，可以通过make_shared函数或者通过构造函数传入普通指针
    * 不要把一个原生指针给多个shared_ptr，不要把this指针交给智能指针管理，这样会重复释放
    * shared_ptr之间的资源共享是通过shared_ptr智能指针拷贝、赋值实现的，因为这样可以引起计数器的更新；而如果直接通过原生指针来初始化，就会导致m_sp和p都根本不知道对方的存在，然而却两者都管理同一块地方
```C++
int* ptr = new int;
shared_ptr<int> p1(ptr);
shared_ptr<int> p2(ptr); //这样不会导致更新，两者不知对方存在
shared_ptr<int> p3(p1);//这样才会导致计数器更新
```

* shared_ptr循环引用导致内存泄漏，引出weak_ptr
    * 循环引用是两个强引用（shared_ptr）互相引用，使得两者的引用计数无法为0，进而无法释放，此时将循环引用的一方变为weak_ptr即可。
```C++
template <typename T>
class Node
{
public:
    Node(const T& value)
        :_pPre(NULL)
        , _pNext(NULL)
        , _value(value)
    {
        cout << "Node()" << endl;
    }
    ~Node()
    {
        cout << "~Node()" << endl;
        cout << "this:" << this << endl;
    }

    shared_ptr<Node<T>> _pPre;
    shared_ptr<Node<T>> _pNext;
    T _value;
};

void Funtest()
{
    shared_ptr<Node<int>> sp1(new Node<int>(1));
    shared_ptr<Node<int>> sp2(new Node<int>(2));

    cout << "sp1.use_count:" << sp1.use_count() << endl;
    cout << "sp2.use_count:" << sp2.use_count() << endl;

    sp1->_pNext = sp2;
    sp2->_pPre = sp1;

    cout << "sp1.use_count:" << sp1.use_count() << endl;
    cout << "sp2.use_count:" << sp2.use_count() << endl;
}
int main()
{
    Funtest();
    system("pause");
    return 0;
}
```
上述情况造成了一个僵局，那就是析构对象时先析构sp2,可是由于sp2的空间sp1还在使用中，所以sp2.use_count减减之后为1，不释放，sp1也是相同的道理，由于sp1的空间sp2还在使用中，所以sp1.use_count减减之后为1，也不释放。sp1等着sp2先释放，sp2等着sp1先释放,二者互不相让，导致最终都没能释放，内存泄漏。

    
* **弱引用（weak_ptr）并不修改该对象的引用计数**，weak_ptr必须从一个share_ptr或另一个weak_ptr转换而来，这也说明，进行该对象的内存管理的是那个强引用的share_ptr,weak_ptr只是提供了对管理对象的一个访问手段这意味这弱引用它并不对对象的内存进行管理。在功能上类似于普通指针，然而一个比较大的区别是，弱引用能检测到所管理的对象是否已经被释放，从而避免访问非法内存
```C++
weak_ptr<Node<T>> _pPre;
weak_ptr<Node<T>> _pNext;
```
expired()用于检测所管理的对象是否已经释放；lock()用于获取所管理的对象的强引用指针，不能直接访问弱引用，需要将其先通过lock转换为强引用再访问

* unique_ptr
    unique_ptr实现独占式拥有或严格拥有概念，保证同一时间内只有一个智能指针可以指向该对象

## 强制转换
C++中强制转换为static_cast, dynamic_cast,const_cast, reinterpret_cast

* static_cast
    * 完成基础数据类型；同一个继承体系中类型的转换；任意类型与空指针类型void* 之间的转换，不能用于普通指针的转换（void空指针除外）
* dynamic_cast
    * 动态类型转换，用于实现RTTI。只能用于含有虚函数的类，用于类层次间的向上和向下转化。只能转指针或引用。向下转化时，如果是非法的对于指针返回NULL，对于引用抛异常bad_cast
* const_cast
    * 用于删除 const、volatile特性
* reinterpret_cast
    * 几乎什么都可以转,不能丢掉 const、volatile特性

## RTTI
运行时类型检查，在C++层面主要体现在dynamic_cast和typeid
> * dynamic_cast
    动态类型转换
> * typeid
    typeid 运算符允许在运行时确定对象的类型，获取对象的实际类型
