# OcrLiteOnnxJvm

#### Demo下载
[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-onnx-jvm/releases)

#### 介绍
* 本项目为java或kotlin通过jni调用OcrLiteOnnx动态运行库的范例。

#### 编译环境
1. jdk 1.8(参考动态运行库编译说明安装)
2. gradle(可由IDE自动安装)

##### 调试&编译说明
1. 开始JVM部分编译之前，请先完成编译OcrLiteOnnx动态运行库，参考[此页面](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/pc_projects/OcrLiteOnnx)的编译说明。
2. 把预先编译好的动态运行库复制到run-test文件夹，根据您选择的编译类型，macOS、linux可能还需要额外配置动态库的搜索路径
3. 从第1步的说明中找到模型下载地址，放到run-test/models文件夹，测试的目标图片放到run-test/test_imgs文件夹
4. 用IDEA打开本项目
5. Main.java为java版调用范例，main.kt为kotlin版调用范例，之后以kotlin为范例来说明
6. IDEA中直接在在mian方法左边点击绿色的运行图标，可以直接调试运行范例，但此时没有输入参数。
* 编辑运行参数：顶部工具栏“运行”图标左边，点开下拉菜单“Edit Configurations”
* VM options:添加```-Djava.library.path=run-test```，把run-test文件夹加到java的lib搜索路径
* Program arguments:添加``run-test/models run-test/test_imgs/1.jpg```，添加命令行输入参数，传入models文件夹和目标图片
* 点击运行，正常的话就可以输出识别结果。
8. 编译为jar包(以Kotlin为例)：在菜单栏找到Project Structure
* 转到“Artifacts”选项卡
* 按“+”号新建配置，Add->Jar->Empty
* 编辑Name:OcrLiteJvm，下面列表左边的名称也会对应改为“OcrLiteOnnxJvm.jar”，这是最终输出的文件名
* 选中左边列表的“OcrLiteOnnxJvm.jar”，底部出现两个按钮，选择“Use Existing Manifest...”，选择src/main/resources/META-INF/MANIFEST.MF
* 展开右边列表，找到OcrLiteOnnxJvm/main/“OcrLiteOnnxJvm.main compile output”，右键“Put into Output Root”
* 选中右边列表的三个Gradle依赖包，分别是“kotlin-stdlib-common:1.3.72”/“kotlin-stdlib-1.3.72”/“annotations:13.0”，右键“Extract Into Output Root”
* 按“OK”关闭项目设置窗口
* 打开菜单栏Build->Build Artifacts，找到刚才的配置，并选Build
* 找到out/artifacts/OcrLiteOnnxJvm/OcrLiteOnnxJvm.jar，并复制到run-test文件夹
* 测试jar包是否正确编译，在run-test文件夹中运行
```
windows:run-test-java.bat
mac或linx:./run-test-java.sh
```

##### 其它问题
如果运行显示错误can’t find dependent libraries
* 检查是否安装C++运行环境，https://support.microsoft.com/zh-cn/help/2977003/the-latest-supported-visual-c-downloads
下载 x64： vc_redist.x64.exe
