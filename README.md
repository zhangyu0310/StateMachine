## StateMachine 有限自动机
系统无关的有限自动机程序，使用C++11和模版编程。经过冒烟测试，能够在windows10平台，MSVC 10.0版本运行。
经过冒烟测试，能够在CentOS系统，g++ (GCC) 4.8.5版本运行。（注：Linux操作系统编译时需要加-lpthread进行编译，否则多线程运行将会抛出异常）
### 特点：
1. 使用模版编程，能够接受各种类型的状态与输入。
2. 可选是否创建新线程进行loop，防止主线程阻塞。
3. 可以任意添加状态、输入与过程处理函数。
4. 可选的默认处理函数，初始化时指定默认处理函数，当遇到不存在的输入，自动执行默认处理函数。
5. 使用C\++11编程，使用C\++11内置的线程与原语，达到系统无关的目的。
6. 基本符合Google的C++编程规范。
### 有待完善的缺点：
1. 不能保证线程安全，使用消息队列存储输入，不能保证读取到的状态为最终状态。
2. 无法删除状态与输入，因为可能涉及到当前状态被删除的情况，处理比较复杂。
3. 部分函数、变量命名糟糕，部分注释语法或用词不当。
4. 添加状态与输入关系的函数，参数略复杂，而且没有默认值，可能修改。
5. 使用过多map，效率可能受到影响。
6. 条件变量编写过于简陋，可能发生彻底阻塞的情况。
7. start函数执行比较生硬，不能加入其他操作，之后可以添加功能，在每次loop之后执行规定动作。
8. 没有使用C++11的移动语义，导致不必要的对象构造析构开销。
### 主要函数说明：
#### init函数：
运行前必须执行init函数进行初始化，以提供初始状态与默认处理函数。
#### receiveImport函数：
向队列插入输入，这里基本无法通过查看状态来确定输入，函数存在线程安全问题。
#### addStateChange函数：
四个参数分别为：当前状态、输入、处理函数、目标状态。目前函数没有默认参数，之后可能考虑增加。
