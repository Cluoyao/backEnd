# C++面向对象知识

> * [内存字节对齐](#内存字节对齐)
> * [面向对象三大特性](#面向对象三大特性)
> * [双冒号、using和namespace](#双冒号using和namespace)
> * [内联函数和函数重载](#内联函数和函数重载)
> * [虚函数可以是内联函数吗](#虚函数可以是内联函数吗)
> * [构造函数/析构函数](#构造函数析构函数)
> * [拷贝构造函数与深浅拷贝](#拷贝构造函数与深浅拷贝)
> * [只在堆上/栈上创建对象](#只在堆上栈上创建对象)
> * [this指针](#this指针)
> * [常函数和常对象](#常函数和常对象)
> * [delete this合法吗](#delete-this合法吗)
> * [为什么空类大小不为0](#为什么空类大小不为0)
> * [静态成员变量与静态成员函数](#静态成员变量与静态成员函数)
> * [能否通过初始化列表初始化静态成员变量](#能否通过初始化列表初始化静态成员变量)
> * [友元全局函数、友元类、友元成员函数](#友元全局函数友元类友元成员函数)
> * [运算符重载及++重载实现](#运算符重载及重载实现)
> * [继承方式、对象模型、同名处理](#继承方式对象模型同名处理)
> * [多继承和菱形继承](#多继承和菱形继承)
> * [静态函数可以是虚函数吗](#静态函数可以是虚函数吗)
> * [类型兼容性原则-为什么会有多态](#类型兼容性原则-为什么会有多态)
> * [重载、重写（覆盖）、重定义](#重载重写重定义)
> * [多态实现的基础](#多态实现的基础)
> * [静态多态和动态多态](#静态多态和动态多态)
> * [虚函数指针和虚函数表](#虚函数指针和虚函数表)
> * [函数指针与指针函数](#函数指针与指针函数)
> * [怎么理解多态和虚函数](#怎么理解多态和虚函数)
> * [构造函数能否实现多态/虚函数指针什么时候初始化](#构造函数能否实现多态虚函数指针什么时候初始化)
> * [构造函数能否是虚函数](#构造函数能否是虚函数)
> * [抽象类和纯虚函数](#抽象类和纯虚函数)
> * [虚析构和纯虚析构](#虚析构和纯虚析构)
> * [为什么析构函数必须是虚函数](#为什么析构函数必须是虚函数)
> * [为什么C++默认的析构函数不是虚函数](#为什么C++默认的析构函数不是虚函数)
> * [类模板和函数模板](#类模板和函数模板)


## [内存字节对齐](https://www.cnblogs.com/jijiji/p/4854581.html)
> * `#pragma pack(n)` 表示的是设置n字节对齐,windows默认是8，linux是4

```C++
struct A{
    char a;
    int b;
    short c;
};
```
> * char占一个字节，起始偏移为零，int占四个字节，min(8,4)=4；所以应该偏移量为4，所以应该在char后面加上三个字节，不存放任何东西，short占两个字节，min(8,2)=2;所以偏移量是2的倍数，而short偏移量是8，是2的倍数，所以无需添加任何字节，所以第一个规则对齐之后内存状态为0xxx|0000|00    
> * 此时一共占了10个字节，但是还有结构体本身的对齐，min(8,4)=4；所以总体应该是4的倍数，所以还需要添加两个字节在最后面，所以内存存储状态变为了 0xxx|0000|00xx，一共占据了12个字节

* 内存对齐规则
    * 对于结构的各个成员，第一个成员位于偏移为0的位置，以后的每个数据成员的偏移量必须是  min(#pragma pack()指定的数,这个数据成员的自身长度)的倍数
    * 在所有的数据成员完成各自对齐之后，结构或联合体本身也要进行对齐，对齐将按照 #pragam pack指定的数值和结构或者联合体最大数据成员长度中比较小的那个，也就是  min(#pragram pack() , 长度最长的数据成员)
    
* 需要对齐的原因
    * 平台原因（移植原因）：不是所有的硬件平台都能访问任意地址上的任意数据，某些硬件平台只能在某些地址处取某些特定类型的数据，否则抛出硬件异常
    * 硬件原因：经过内存对齐之后，CPU的内存访问速度大大提升。访问未对齐的内存，处理器要访问两次（数据先读高位，再读低位），访问对齐的内存，处理器只要访问一次，为了提高处理器读取数据的效率，我们使用内存对齐

## 面向对象三大特性
通过类创建一个对象的过程叫实例化，实例化后使用对象可以调用类成员函数和成员变量，其中类成员函数称为行为，类成员变量称为属性。类和对象的关系：类是对象的抽象，对象是类的实例

* 封装
    * 把客观事物封装成抽象的类，并且类可以把自己的数据和方法只让可信的类或者对象操作，对不可信的进行信息隐藏。
    * public，private，protected
* 继承
    * 基类（父类）——> 派生类（子类）
* 多态



## 双冒号、using和namespace
* namespace主要用来解决命名冲突的问题
    * 必须在全局作用域下声明
    * 命名空间下可以放函数，变量、结构体和类
    * 命名空间可以嵌套命名空间
    * 命名空间是开放的，可以随时加入新成员（添加时只需要再次声明namespace，然后添加新成员即可
    
* 双冒号::作用域运算符
    * 全局作用域符（::name）：用于类型名称（类、类成员、成员函数、变量等）前，表示作用域为全局命名空间
    * 类作用域符（class::name）：用于表示指定类型的作用域范围是具体某个类的
    * 命名空间作用域符（namespace::name）:用于表示指定类型的作用域范围是具体某个命名空间的

* using分为using声明和using编译指令
    * `using std::cout; //声明`
    * `using namespace std; //编译指令`
    * 尽量使用声明而不是编译指令，不同命名空间中可能会有相同的变量名，编译指令执行两个命名空间后，会产生二义性

## 内联函数和函数重载
* 内联函数
    * 相当于把内联函数里面的内容写在调用内联函数处；
    * 相当于不用执行进入函数的步骤，直接执行函数体；
    * 相当于宏，却比宏多了类型检查，真正具有函数特性；
    * 不能包含循环、递归、switch 等**复杂操作**；
    * 在类声明中定义的函数，除了虚函数的其他函数都会自动隐式地当成内联函数，内联函数对于编译器而言只是一个建议，编译器不一定会接受这种建议，即使没有声明内联函数，编译器可能也会内联一些小的简单的函数。  
    * 补充：**内联机制用于优化规模较小、流程直接、频繁调用的函数**
    
* C++的函数名称可以重复，称为函数重载。
    * 其中必须在同一作用域下的函数名称相同，不能是一个在全局，一个局部，或者不同的代码块中
    * 可以根据函数参数的个数、类型（const也可以作为重载条件）、顺序不同进行函数重载，但**不能用函数返回值进行重载**
    * 当函数重载遇到函数默认参数时，要注意二义性。

## 虚函数可以是内联函数吗
* 虚函数可以是内联函数，内联是可以修饰虚函数的，但是当虚函数表现多态性的时候不能内联。
* 内联是在编译期内联，而虚函数的多态性在运行期，编译器无法知道运行期调用哪个代码，因此虚函数表现为多态性时（运行期）不可以内联。
* inline virtual 唯一可以内联的时候是：编译器知道所调用的对象是哪个类（如 Base::who()），这只有在编译器具有实际对象而不是对象的指针或引用时才会发生。

## 构造函数/析构函数
构造函数和析构函数，分别对应变量的初始化和清理，变量没有初始化，使用后果未知；没有清理，则会内存管理出现安全问题。
当对象结束其生命周期，如对象所在的函数已调用完毕时，系统会自动执行析构函数
> * 构造函数：与类名相同，没有返回值，不写void，**可以发生重载**，可以有参数，编译器自动调用，只调用一次。
> * 析构函数：~类名，没有返回值，不写void，**不可以发生重载**，不可以有参数，编译器自动调用，只调用一次。

* 构造函数
    * 系统会默认给一个类提供三个函数：默认构造函数（无参，函数体为空）、默认拷贝构造和析构函数（无参，函数体为空），其中默认拷贝构造可以实现简单的值拷贝。
    * 提供了有参构造函数，就不提供默认构造函数；提供了拷贝构造函数，就不会提供其他构造函数，若自己定义可有参构造，也需要自定义无参构造函数

* 析构函数
    * 如果一个类中有指针，且在使用的过程中动态的申请了内存，那么最好自定义析构函数，在销毁类之前，释放掉申请的内存空间，避免内存泄漏 

## 拷贝构造函数与深浅拷贝
拷贝构造函数的参数必须加const，因为防止修改，本来就是用现有的对象初始化新的对象。

* 拷贝构造函数的使用时机
    * 使用已经创建好的对象初始化新对象 `A a; A b = a; A c(a); b = c;//b = c不是初始化，调用赋值运算符 `
    * 以值传递的方式来给函数参数传值
    * 以值方式返回局部对象（不常用，一般不返回局部对象） 


* 深拷贝和浅拷贝
只有当对象的成员属性在堆区开辟空间内存时，才会涉及深浅拷贝，如果仅仅是在栈区开辟内存，则默认的拷贝构造函数和析构函数就可以满足要求。
    * **浅拷贝**：使用默认拷贝构造函数，拷贝过程中是按字节复制的，对于指针型成员变量只复制指针本身，而不复制指针所指向的目标，因此涉及堆区开辟内存时，会将两个成员属性指向相同的内存空间，从而在释放时导致内存空间被多次释放，使得程序down掉。
    * **浅拷贝的问题**：当出现类的等号赋值时，系统会调用默认的拷贝函数——即浅拷贝，它能够完成成员的一一复制。当数据成员中没有指针时，浅拷贝是可行的。但当数据成员中有指针时，如果采用简单的浅拷贝，则两类中的两个指针将指向同一个地址，当对象快结束时，会调用两次free函数，指向的内存空间已经被释放掉，再次free会报错；另外，一片空间被两个不同的子对象共享了，只要其中的一个子对象改变了其中的值，那另一个对象的值也跟着改变了所以，这时，必须采用深拷贝 
    * **深拷贝**：自定义拷贝构造函数，在堆内存中另外申请空间来储存数据，从而解决指针悬挂的问题。**需要注意自定义析构函数中应该释放掉申请的内存** 

我们在定义类或者结构体的时候，**最后都要重写拷贝函数，避免浅拷贝这类不易发现但后果严重的错误产生**

## 只在堆上/栈上创建对象
> * **只能在堆上生成对象：将析构函数设置为私有。**
原因：C++是静态绑定语言，编译器管理栈上对象的生命周期，编译器在为类对象分配栈空间时，会先检查类的析构函数的访问性。若析构函数不可访问，则不能在栈上创建对象。
> * **只能在栈上生成对象：将new 和 delete 重载为私有。**
原因：在堆上生成对象，使用new关键词操作，其过程分为两阶段：第一阶段，使用new在堆上寻找可用内存，分配给对象；第二阶段，调用构造函数生成对象。
将new操作设置为私有，那么第一阶段就无法完成，就不能够再堆上生成对象。

## this指针
* **为什么会有this指针**
在类实例化对象时，只有非静态成员变量属于对象本身，剩余的静态成员变量，静态函数，非静态函数都不属于对象本身，因此非静态成员函数只会实例一份，多个同类型对象会共用一块代码，由于类中每个实例后的对象都有独一无二的地址，因此不同的实例对象调用成员函数时，函数需要知道是谁在调用它，因此引入了this指针。
* **this指针的作用**
this指针是隐含在对象成员函数内的一种指针。当一个对象被创建后，它的每一个成员函数都会含有一个系统自动生成的隐含指针this。this指针指向被调用的成员函数所属的对象（谁调用成员函数，this指向谁），*this表示对象本身，**非静态成员函数中才有this，静态成员函数内部没有**。
    * this 并不是一个常规变量，而是个右值，所以不能取得 this 的地址（不能 &this）。
    * 对非静态成员函数默认添加了this指针，类型为classname *const this
* **this指针使用**
    * 当形参与成员变量名相同时，用this指针来区分
    * 为实现对象的链式引用，在类的非静态成员函数中返回对象本身，可以用return *this，this指向对象，/*this表示对象本身。 

## 常函数和常对象
` void func() const //常函数，此处func为类成员函数`
` const Person p2; //常对象`

* 常函数修饰的是this指针，不允许修改this指针指向的值，如果执意要修改常函数，可以在成员属性前加**mutable**。
* 常对象不允许修改属性，不可以调用普通成员函数，可以调用常函数。

## delete this合法吗

合法，但有前提：

* 必须保证 this 对象是通过 new（不是 new[]、不是 placement new、不是栈上、不是全局、不是其他对象成员）分配的
* 必须保证调用 delete this 的成员函数是最后一个调用 this 的成员函数
* 必须保证成员函数的 delete this 后面没有调用 this 了
* 必须保证 delete this 后没有人使用了

## 为什么空类大小不为0
sizeof(空class) = 1，为了确保两个不同对象的地址不同。

## 静态成员变量与静态成员函数
若将成员变量声明为static，则为静态成员变量，与一般的成员变量不同，无论建立多少对象，都只有一个静态成员变量的拷贝，静态成员变量属于一个类，所有对象共享。静态变量在编译阶段就分配了空间，对象还没创建时就已经分配了空间，放到全局静态区。

* 静态成员变量
    * 最好是类内声明，类外初始化（以免类名访问静态成员访问不到）
    * 无论公有，私有，静态成员都可以在类外定义，但私有成员仍有访问权限
    * 非静态成员类外不能初始化
    * 静态成员数据是共享的。 
* 静态成员函数
    * 静态成员函数可以直接访问静态成员变量，不能直接访问普通成员变量，但可以通过参数传递的方式访问
    * 普通成员函数可以访问普通成员变量，也可以访问静态成员变量
    * 静态成员函数没有this指针。非静态数据成员为对象单独维护，但静态成员函数为共享函数，无法区分是哪个对象，因此不能直接访问普通变量成员，也没有this指针。

## 能否通过初始化列表初始化静态成员变量
不能，静态成员变量最好类内声明，类外初始化.静态成员是单独存储的，并不是对象的组成部分。如果在类的内部进行定义，在建立多个对象时会多次声明和定义该变量的存储位置。在名字空间和作用域相同的情况下会导致重名的问题。

## [友元全局函数、友元类、友元成员函数](https://www.cnblogs.com/qinguoyi/p/10254263.html)
友元主要是为了访问类中的私有成员（包括属性和方法），会破坏C++的封装性，尽量不使用

* 友元全局函数
    * 友元函数声明可以在类中的任何地方，一般放在类定义的开始或结尾
    * 一个函数可以是多个类的友元函数，只需要在各个类中分别声明
    * 友元函数在类内声明，类外定义，定义和使用时不需加作用域和类名，与普通函数无异。 
* 友元类
    * 友元不可继承
    * 友元是单向的，类A是类B的友元类，但类B不一定是类A的
    * 友元不具有传递性，类A是类B的友元类，类B是类C的友元类，但类A不一定是类C的友元类。

* 友元成员函数
    * 使类B中的成员函数成为类A的友元函数，这样类B的该成员函数就可以访问类A的所有成员

## 运算符重载及++重载实现
### 运算符重载基本属性

* 运算符重载的目的是扩展C++中提供的运算符的适用范围，使之能作用于对象，或自定义的数据类型
* 运算符重载的实质是函数重载，可以重载为普通成员函数，也可以重载为成员函数
* 运算符重载也是多态的一种，和函数重载称为静态多态，表示函数地址早绑定，在编译阶段就确定好了地址

### 运算符重载总结

* 重载运算符()，[] ，->， =的时候，运算符重载函数必须声明为类的成员函数
* 重载运算符<<，>>的时候，运算符只能通过全局函数配合友元函数进行重载
* 不要重载&&和||运算符，因为无法实现短路原则。

### i++和++i实现
C++内置类型的后置++返回的是变量的拷贝，也就是不可修改的值；前置++返回的是变量的引用，因此可以作为修改的左值。即++（++a）或（++a）++都可以，但++（a++）不可以，（C++默认必须修改a的值，如果不修改则报错）。
```C++
//++i
int&  int::operator++()
{
    *this +=1；
    return *this；
}

//i++
const int  int::operator(int)
{
    int oldValue = *this；
    ++（*this）；
    return oldValue；
}
```

## [继承方式、对象模型、同名处理](https://www.cnblogs.com/qinguoyi/p/10277350.html)
继承主要是为了减少代码的重复内容，解决代码复用问题。通过抽象出一个基类（父类），将重复代码写到基类中，在派生类（子类）中实现不同的方法。

### 继承方式

* 公有继承：保持父类中的访问属性
* 私有继承：将父类中的所有访问属性改为private
* 保护继承：除父类中的私有属性，其他改为保护属性

### 继承的对象模型

* 子类中会继承父类的私有成员，只是被编译器隐藏起来了，无法访问到，通过sizeof(子类class)可以检查出。
* 子类创建对象时，先调用父类的构造函数，然后再调用自身的构造，析构顺序与构造顺序相反
    * 由于继承中父类和子类的构造、析构顺序原因，当父类中只提供了有参构造（默认构造等函数会被隐藏），而子类仅仅调用默认构造时，会因为子类创建对象时无法调用父类构造函数而报错，这里可以让子类利用初始化列表来显式调用父类有参构造函数来进行父类构造，然后进行子类构造。 
* 子类会继承父类的成员属性和成员函数，但子类不会继承父类构造函数和析构函数

### 继承中的同名处理
* 父类和子类**成员属性**同名，用子类声明对象调用子类属性，若想调用父类成员，则加上父类的作用域
* 父类和子类**成员函数**同名，子类函数不会覆盖父类的成员，只是隐藏起来，用子类声明对象调用子类成员函数，若想调用父类函数（包括重载），则加上父类的作用域
* 若子类中没有与父类同名的成员函数，子类声明对象后，可以直接调用父类成员函数。

## [多继承和菱形继承](https://www.cnblogs.com/qinguoyi/p/10277350.html)
### 多继承
多继承会产生二义性的问题。如果继承的多个父类中有同名的成员属性和成员函数，在子类调用时，需要指定作用域从而确定父类。

### 菱形继承
两个子类继承于同一个父类，同时又有另外一个类多继承于两个子类，这种继承称为菱形继承。比如羊和驼继承于动物类，同时羊驼继承于羊和驼。

#### 菱形继承会产生问题
* **浪费空间。**羊驼继承了两份动物类中的某些数据和函数，但只需要一份即可
* **二义性。从不同途径继承来的同名的数据成员在内存中有不同的拷贝造成数据不一致问题。** 羊驼调用数据和函数时，会出现二义性，通过sheep类得到一个age，通过carmel类得到一个age，两个数据不会相互影响，相互修改，导致同一份数据不一致。


#### 解决菱形继承的问题
使用虚继承，在**继承方式前加virtual**，这样的话羊驼可以直接访问m_Age，不用添加作用域，且这样操作的是共享的一份数据
```C++
class Animal{
public:
    int m_Age;
};
class Sheep:virtual public Animal{
    int m_sheep;
};
class Camel :virtual public Animal{
    int m_camel;
};

class Son :public Sheep, public Camel{
    int m_son
};
void test01(){
    Son son;
    son.m_Age = 10;
    cout << sizeof(Animal) << endl; //m_Age
    cout << sizeof(Sheep) << endl;  //sheep-Vbptr,m_sheep,m_Age
    cout << sizeof(Camel) << endl;  //camel-Vbptr,m_camel,m_Age
    cout << sizeof(Son) << endl;    //sheep-Vbptr,m_sheep,camel-Vbptr,m_camel,m_son,m_Age
}
```
<div align=center><img width="640" height="450" src="https://github.com/qinguoyi/backEnd/blob/master/utils/%E8%8F%B1%E5%BD%A2%E7%BB%A7%E6%89%BF.png"/></div>

> * **特别注意：**此时son没有自己的虚基类表和虚基类指针，只是继承了sheep和camel的虚基类指针和虚基类表，只是修改了两个虚基类表中的值，修改为当前类中，如何通过继承的虚基类指针查找虚基类数据
> * Son继承Sheep父类，父类中有虚基类指针vbptr(virtual base pointer)，对象结构类似结构体，首元素是虚基类指针，其余为自身数据（不包括静态成员和成员函数）
> * Sheep的虚指针指向下面Sheep的虚基类表vbtale@Sheep(virtual base table)，虚基类表是一个整型数组，数组第二个元素值为20，即Sheep的虚指针地址偏移20指向Animal的m_Age地址。Camel父类同理，因此，类中只有一个m_Age元素。
> * Son中包含了两个指针和四个int类型，所以大小为24。

```C++
class Animal{
public:
    int m_Age;
};
class Sheep:virtual public Animal{
    int m_sheep;
};
class Camel :virtual public Animal{
    int m_camel;
};

class Son :virtual public Sheep, virtual public Camel{
    int m_son
};
void test01(){
    Son son;
    son.m_Age = 10;
    cout << sizeof(Animal) << endl; //m_Age
    cout << sizeof(Sheep) << endl;  //sheep-Vbptr,m_sheep,m_Age
    cout << sizeof(Camel) << endl;  //camel-Vbptr,m_camel,m_Age
    cout << sizeof(Son) << endl;    //son-vbptr,m_son,m_Age,sheep-Vbptr,m_sheep,camel-Vbptr,m_camel,
}
```

<div align=center><img width="640" height="600" src="https://github.com/qinguoyi/backEnd/blob/master/utils/%E5%A4%9A%E8%99%9A%E7%BB%A7%E6%89%BF.png"/></div>

> * 注意跟上面的区别，一个是son类中的元素顺序，一个是son类有了自己的虚基类指针和虚基类表

* 虚继承
    * 一般通过虚基类指针和虚基类表实现，将共同基类设置为虚基类
    * **每个虚继承的子类（虚基类本身没有）**都有一个虚基类指针（占用一个指针的存储空间）和虚基类表（不占用类对象的存储空间），**虚基类指针属于对象，虚基类表属于类**
    * 当虚继承的子类被当做父类继承时，虚基类指针也会被继承。
    * 虚表中只记录了虚基类数据在派生类对象中与派生类对象首地址(虚基类指针)之间的偏移量,以此来访问虚基类数据
    * 虚继承不用像普通多继承那样维持着公共基类（虚基类）的两份同样的拷贝，节省了存储空间。
    * 虚基类表本质是一个**整型数组**

## 静态函数可以是虚函数吗
不可以，因为虚函数属于对象，不属于类，静态函数属于类

## 类型兼容性原则 为什么会有多态
类型兼容规则是指在需要基类对象的任何地方，都可以使用公有派生类的对象来替代,如使用子类对象可以直接赋值给父类对象或子类对象可以直接初始化父类对象时，**对于同样的一条语句，不管传入子类还是父类对象，都是调用的父类函数，但我们想实现的是同样的一条语句，传入不同的对象，调用不同的函数**.
```C++
class Animal{
public:
	void speak(){
		cout << "Animal speak" << endl;
	}
};

class Sheep :public Animal{
public:
	void speak(){ //重定义，子类重新定义父类中有相同名称的非虚函数 
		cout << "Sheep speak" << endl;
	}
};

void doSpeak(Animal &animal){
	animal.speak();
}

//想通过父类引用指向子类对象
void test01(){
	Sheep sheep;
	doSpeak(sheep); //Animal speak;
	sheep.speak();  //sheep speak
	sheep.Animal::speak();  //Animal speak; //继承中的重定义可以通过作用域
}
```

但我们想**传入子类对象调用子类函数，传入父类对象调用父类函数**，即同样的调用语句有多种不同的表现形态，这样就出现了**多态**

## 重载、重写、重定义

* 重载：是函数名相同，参数列表不同。重载只是在同一个类的内部存在，但是不能靠返回类型来判断
* 重写：也叫做覆盖。子类重新定义父类中有相同名称和参数的虚函数。两者的函数特征相同。
    * 被重写的函数不能是static的。必须是virtual的
    * 重写函数必须有相同的类型，名称和参数列表
    * 重写函数的访问权限可以不同。尽管virtual是private的，子类中重写改写为public,protected也是可以的。 
* 重定义：也叫做隐藏。子类重新定义父类中有相同名称的非虚函数 ( 参数列表可以不同 ) 。如果一个类，存在和父类相同的函数，那么，这个类将会覆盖其父类的方法，除非你在调用的时候，强制转换为父类类型或加上父类作用域

## 多态实现的基础
> * 继承
> * 虚函数重写
> * 父类指针或引用指向子类对象访问虚函数

```C++
class Animal{
public:
	virtual  void speak(){ //在父类中声明虚函数，可以实现多态，动态联编
		cout << "Animal speak" << endl;
	}
	int m_age = 0;
};

class Sheep :public Animal{
public:
	void speak(){ //发生多态时，子类对父类中的成员函数进行重写，virtual可写可不写
		cout << "Sheep speak" << endl;
	}
	int m_age = 1;
};

void doSpeak(Animal &animal){
	animal.speak();
}

void test01(){
    //传入子类对象调用子类成员函数
	Sheep sheep;
	doSpeak(sheep); //sheep speak;
	
    //子类对象直接调用子类成员函数
	sheep.speak();  //sheep speak;
	
	//子类对象通过作用域调用父类成员函数
    sheep.Animal::speak();  //animal speak;
    
    //基类成员不能转换为子类成员，即不能向下转换
	//Animal *animal0 = new Animal();
	//Sheep * sheep0 = animal0;
	//sheep0->speak();

    //同样不能向下转换
	//Animal animal0;
	//Sheep sheep0 = animal0;

    //父类指针指向子类对象
	Sheep *sheep1 = new Sheep();
	Animal *animal1 = sheep1;
	animal1->speak(); //sheep speak;
	
	//父类引用指向子类对象
	Sheep sheep2;
	Animal &animal2 = sheep2;
	animal2.speak();    //sheep speak;
	
	//子类对象直接赋值给父类对象，不符合多态条件，符合类型兼容性原则
	Sheep sheep0;
	Animal animal0 = sheep0;
	animal0.speak();    //animal speak;
}
```

## 静态多态和动态多态
> * 静态多态（运算符重载、函数重载）
> * 动态多态（继承、虚函数）

两者主要的区别：函数地址是早绑定（静态联编）还是晚绑定（动态联编）。即，在编译阶段确定好地址还是在运行时才确定地址。

## 虚函数指针和虚函数表
> * 前提发生了多态，每个类中都有虚函数表，最开始的父类创建虚函数表，后面的子类继承父类的虚函数表，然后对虚函数重写
> * 虚函数重写（覆盖）的实质就是重写父类虚函数表中的父类虚函数地址；
> * 实现多态的流程：虚函数指针->虚函数表->函数指针->入口地址，**虚函数表（vftable）属于类**，或者说这个类的所有对象共享一个虚函数表；**虚函数指针（vfptr）属于单个对象**。
> * 在程序调用时，先创建对象，编译器在对象的内存结构头部添加一个虚函数指针，进行动态绑定，虚函数指针指向对象所属类的虚函数表。
> * 虚函数表是一个指针数组，其元素是虚函数的指针，每个元素对应一个函数的指针。如果子类对父类中的一个或多个虚函数进行重写，子类的虚函数表中的元素顺序，会按照父类中的虚函数顺序存储，之后才是自己类的函数顺序。
> * 编译器根本不会去区分，传进来的是子类对象还是父类对象，而是关心调用的函数是否为虚函数。如果是虚函数，就根据不同对象的vptr指针找属于自己的函数。父类对象和子类对象都有vfptr指针，传入对象不同，编译器会根据vfptr指针，到属于自己虚函数表中找自己的函数。即：vptr--->虚函数表------>函数的入口地址，从而实现了迟绑定(在运行的时候，才会去判断)。

## [函数指针与指针函数](https://www.cnblogs.com/qinguoyi/p/10198019.html)
* 指针函数`int* f(int x, int y)`本质是函数，返回值为指针，函数指针`int (*f)(int x)`本质是指针，指向函数的指针

* 通常我们可以将指针指向某类型的变量，称为类型指针（如，整型指针）。若将一个指针指向函数，则称为函数指针。

* 函数名代表函数的入口地址，同样的，我们可以通过根据该地址进行函数调用，而非直接调用函数名。
```C++
void test001(){
    printf("hello, world");
}

int main(){
    void(*myfunc)() = test001;//将函数写成函数指针
    myfunc(); //调用函数指针 hello world
}
```
test001的函数名与myfunc函数指针都是一样的，即都是函数指针。test001函数名是一个函数指针常量，而myfunc是一个函数指针变量，这是它们的关系。

* 函数指针多用于回调函数，回调函数最大的优势在于灵活操作，可以实现用户定制的函数，降低耦合性，实现多样性，如STL中



## 怎么理解多态和虚函数
* 多态的实现主要分为静态多态和动态多态，静态多态主要是重载，在编译的时候就已经确定；动态多态是用虚函数机制实现的，在运行期间动态绑定。

* 举个例子：一个父类类型的指针指向一个子类对象时候，使用父类的指针去调用子类中重写了的父类中的虚函数的时候，会调用子类重写过后的函数

虚函数的实现：在有虚函数的类中，类的最开始部分是一个虚函数表的指针，这个指针指向一个虚函数表，表中放了虚函数的地址，实际的虚函数在代码段(.text)中。当子类继承了父类的时候也会继承其虚函数表，当子类重写父类中虚函数时候，会将其继承到的虚函数表中的地址替换为重新写的函数地址。使用了虚函数，会增加访问内存开销，降低效率。

## 构造函数能否实现多态/虚函数指针什么时候初始化
两个问题本质是一样的，构造函数不能实现多态

* 对象在创建时,由编译器对VPTR指针进行初始化，只有当对象的构造完全结束后VPTR的指向才最终确定。

* 子类中虚函数指针的初始化过程
当定义一个子类对象的时候比较麻烦，因为构造子类对象的时候会首先调用父类的构造函数然后再调用子类的构造函数。当调用父类的构造函数的时候，此时会创建Vptr指针，该指针会指向父类的虚函数表；然后再调用子类的构造函数，子类继承父类的虚函数指针，此时Vptr又被赋值指向子类的虚函数表。 

## 构造函数能否是虚函数
不能，因为在调用构造函数时，虚表指针并没有在对象的内存空间中，必须要构造函数调用完成后才会形成虚表指针

## 抽象类和纯虚函数
在程序设计中，如果仅仅为了设计一些虚函数接口，打算在子类中对其进行重写，那么不需要在父类中对虚函数的函数体提供无意义的代码，可以通过纯虚函数满足需求。
 
* 纯虚函数的语法格式：`virtual 返回值类型 函数名 () = 0; `只需要将函数体完全替换为 =0即可，**纯虚函数必须在子类中进行实现**，在子类外实现是无效的。

* 注意
    * 如果父类中出现了一个纯虚函数，则这个类变为了抽象类，抽象类不可实例对象
    * 如果父类为抽象类，子类继承父类后，必须实现父类所有的纯虚函数，否则子类也为抽象类，也无法实例对象**但纯虚析构函数例外，因为子类不会继承父类的析构函数**

## 虚析构和纯虚析构
> * 仅仅发生继承时，创建子类对象后销毁，函数调用流程为：父类构造函数->子类构造函数->子类析构函数->父类析构函数；
> * 当发生多态时（父类指针或引用指向子类对象），通过父类指针在堆上创建子类对象，然后销毁，调用流程为：父类构造函数->子类构造函数->父类析构函数，不会调用子类析构函数，因此子类中会出现内存泄漏问题。

解决方法：将父类中的析构函数设置为虚函数，设置后会先调用子类析构函数，再调用父类析构函数

* 纯虚析构
    * 纯虚析构需要类内声明，类外实现
    * 纯虚析构也是虚函数，该类也为抽象类
    * 子类不会继承父类的析构函数，当父类纯虚析构没有实现时，子类不是抽象类，可以创建创建对象。
    
## 为什么析构函数必须是虚函数
因为当发生多态时，父类指针在堆上创建子类对象，销毁时会内存泄漏

## 为什么C++默认的析构函数不是虚函数
因为虚函数需要额外的虚函数表和虚表指针，占用额外的内存。而对于不会被继承的类来说，其析构函数如果是虚函数，就会浪费内存 

## 类模板和函数模板
通过template<class T>或template<typename T>实现，主要用于数据的类型参数化，简化代码，有类模板和函数模板，函数模板是用于生成函数的，类模板则是用于生成类的

* 类模板和函数模板定义
> * template声明下面是函数定义，则为函数模板，否则为类模板。
> * 注意：每个函数模板前必须有且仅有一个template声明，不允许多个template声明后只有一个函数模板，也不允许一个template声明后有多个函数模板(类模板同理)。

* 类模板与函数模板的区别
    * 类模板不支持自动类型推导
    * 数据类型可以有默认参数.
