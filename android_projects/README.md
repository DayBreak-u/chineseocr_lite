## 以下内容仅代表benjaminwan个人观点 仅供参考

#### android平台onnxruntime vs ncnn对比测试介绍
1. OcrLiteAndroidOnnx项目使用onnxruntime框架，OcrLiteAndroidNcnn项目使用ncnn框架，用于对比的项目JNI部分均使用C++编写，JVM部分使用Kotlin编写。
2. 除了调用推理框架的代码，其它代码尽量保持一致。
3. 使用chineseocr-lite的onnx分支的模型，ncnn框架使用专用的模型格式，由同一个模型从onnx格式转为ncnn专用格式。
4. 20210102更新：除了对比CPU计算的结果，因ncnn支持vulkan跨平台GPU加速，故加入ncnn GPU加速测试结果。
5. 对比测试的项目有：推理速度、编译后apk文件大小
6. 对比用的demo下载(v1.5.2)：[onnx版](https://gitee.com/benjaminwan/ocr-lite-android-onnx/releases)，[ncnn版](https://gitee.com/benjaminwan/ocr-lite-android-ncnn/releases)

#### 推理速度对比
1. 20210102更新：在同一手机上进行测试，上次测试有PC模拟器但非典型应用场景故此次取消。
2. 测试硬件：华为荣耀play1，麒麟970，4*Cortex-A73+4*Cortex-A53，系统版本Android 9
3. 20210102更新：麒麟970的GPU为Mali-G72@850M，12核
4. onnxruntime版本为1.6.0，ncnn版本20201218, opencv 3.4.10
5. 20210102更新：由前次的测试结果可知，线程数为4时，耗时最少，故此次均采用4线程，选取了640x640的普通图片来测试，测试100个循环取平均值
6. 20201130更新：除了推理框架使用OpenMP以外，Demo程序的AngleNet和CrnnNet阶段也使用OpenMP来加速计算。实测OpenMP对DbNet阶段没有加速效果，因为DbNet阶段是单任务单线程的。
7. 20210102更新：ncnn vulkan版，DbNet阶段使用GPU加速，AngleNet和CrnnNet阶段仍然使用CPU+OpenMP多线程计算。
* DbNet阶段特别适合GPU加速，GPU只有compute queue的概念没有线程，低端的GPU只有1个compute queue。
* 在代码改动小的前提下，实测AngleNet和CrnnNet使用CPU多线程计算更快。
* 如果重新设计架构，比如CPU x个线程+GPU 1~N个任务，这样并行计算，理论上行得通，但整体改动很大。
* 因为AngleNet和CrnnNet的数据量小，计算耗时小，即使通过复杂的算法和架构实现提升，也对整体的结果影响小，投入产出比小。
* 这样的架构好处是：GPU和CPU轮流使用，不容易过热并可以让GPU和CPU都保持在高效的工作频率。代码改动小、程序结构简单。

| 版本    | ABI  | 线程数 | dbNet耗时 | 总耗时 |
| ------- | ------- | ----: | ----: | ----: |
| onnx      | arm64-v8a | 4 | 658ms | 790ms |
| ncnn(cpu) | arm64-v8a | 4 | 541ms | 756ms |
| ncnn(gpu) | arm64-v8a | 4 | 209ms | 455ms |

#### 推理速度对比总结：
1. onnx与ncnn(cpu)速度差不多。
2. onnx在crnn阶段有优势，因为ncnn在crnn阶段是外循环。
3. ncnn在模型上有优化，也会导致计算负载较低。
4. 通过logcat观察，在cpu运算的情况下，大约50循环后，cpu降频，耗时增加。
5. ncnn(gpu)速度最快，原因就如前面分析的。

#### 文件大小对比
1. onnxruntime为自己编译的动态库，ncnn为静态库。
2. onnxruntime的大小还能通过自己编译mini_build选项进一步优化，但因为模型格式也得转为ort专用格式，故这里就没有加入对比。
3. ncnn(cpu)代表不带vulkan支持的ncnn，ncnn(gpu)代表带vulkan支持的ncnn。
4. 因android7才能支持vulkan，所以ncnn(gpu)要求api24，且因为api24的编译打包签名升级为v2，再加上vulkan支持增大了不少体积。
5. 因为ncnn模型文件也更小，所以这部分稍有点优势。
6. 直接对比编译后apk大小。

| 平台     | onnx大小 | ncnn(cpu) | ncnn(gpu) |
| ------- | -------- | ------: | ------: |
| android |  34M  |  21M |   58M |


#### 文件大小对比总结：
1. ncnn(cpu)在android平台apk文件大小小于onnx版。
2. ncnn(gpu)在apk大小没有优势，但推理速度最快，如果为了快而牺牲体积，可以考虑ncnn(gpu)版。

#### 总体总结：
1. ncnn为移动端优化，模型大小和文件大小都更小，适合移动端部署。
2. ncnn在移动端的推理速度更快。