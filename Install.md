# 简介
&emsp;&emsp;RubberDuck提供了仿真运行调度的总控程序以及随机变量生成、数据结果统计等公共子程序，用户需要根据仿真概念模型，按照RubberDuck接口规范和设计开发仿真模型程序，并通过C++编译系统与RubberDuck仿真库链接形成最终的仿真模型可执行程序。
# 编译
&emsp;&emsp;RubberDuck采用GNU的Makefile进行编译。如果需要也可以采用Eclipse等集成开发环境进行开发，但需要配置include、library目录和文件选项。RubberDuck采用GNU的Makefile已经包含了库的编译和生成规则，RubberDuck下载完成后，进入RubberDuck目录，在命令行中输入``make all``命令，将在bin目录中自动生成库文件和Examples目录下的仿真模型示例的可执行文件。如果需要清除已编译的目标文件，在命令行中可以输入``make clean``命令。
# 模型开发
&emsp;&emsp;如果需要开发自己的仿真模型，可以通过以下步骤开发和运行仿真模型：
&emsp;&emsp;（1）在RubberDuck的examples目录下新建目录如：project1
&emsp;&emsp;（2）按照概念模型选择仿真调度策略；
&emsp;&emsp;（3）创建模型程序文件名如：model1.cpp；
&emsp;&emsp;（4）根据概念模型设计和RubberDuck接口规范进行模型程序设计与开发；
&emsp;&emsp;（5）拷贝示例中的Makefile文件到project1目录；
&emsp;&emsp;（6）将Makefile文件的目标依赖文件OBJS改为模型单元处理程序文件：model1.o，将输出的可执行文件EXECUTABLE改为期望输出的可执行文件名；
&emsp;&emsp;（7）在project1工程目录中执行``make all``命令即可在RubberDuck的bin目录中编译生成指定名称的可执行程序。
&emsp;&emsp;如果不在RubberDuck的examples目录下新建自己的工程目录，则需要调整Makefile文件的包含路径和链接路径以指向正确的RubberDuck目录。
&emsp;&emsp;RubberDuck当前兼容Windows和Linux操作系统。Windows环境下需要安装Mingw C++开发环境。如果开发环境未包含make.exe文件，可以将Mingw中bin目录下的mingw32-make.exe文件拷贝一份改为make.exe以支持C++程序的自动编译生成。

Enjoy working with RubberDuck!