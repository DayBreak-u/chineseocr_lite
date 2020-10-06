ncnn版本维护修改

修改了 命令行接口，添加了使用示例。

添加 新的库函数，方便其它程序调用。

主程序改为调用新加的库函数，以演示使用方法。



目前编译环境：

Windows:

visual studio 2019 +ncnn 最新版本。
在Windows下平台下，准备好库后，直接用vs2019打开cmake的方法打开cmakelist.txt 文件即可编译。

Linux:

cmake +ncnn 最新版本

请修改目录下的CMakeList.txt 以适配你的环境，主要是opencv及vulkan的头文件或目录路径。



动态库支持Windows /Linux
