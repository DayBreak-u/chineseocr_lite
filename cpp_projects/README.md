## 以下内容仅代表benjaminwan个人观点 仅供参考

#### PC平台onnxruntime vs ncnn对比测试介绍

1. OcrLiteOnnx项目使用onnxruntime框架，OcrLiteNcnn项目使用ncnn框架，用于对比的项目均使用C++编写。
2. 除了调用推理框架的代码，其它代码尽量保持一致。
3. 使用chineseocr-lite的onnx分支的模型，ncnn框架使用专用的模型格式，由同一个模型从onnx格式转为ncnn专用格式。
4. 20210102更新：除了对比CPU计算的结果，因ncnn支持vulkan跨平台GPU加速，故加入ncnn GPU加速测试结果。
5. 对比测试的项目：推理速度
6. 对比用的demo下载：[onnx版](https://gitee.com/benjaminwan/ocr-lite-onnx/releases)，[ncnn版](https://gitee.com/benjaminwan/ocr-lite-ncnn/releases)

#### 推理速度对比

1. 在同一台真机上的mac和win双系统中分别测试。
2. 测试硬件：NUC8I7HVK冥王峡谷(i7-8809G) 32GRAM 2TSSD
3. 冥王峡谷有2个GPU，GPU0是Radeon RX Vega M(性能相当于GTX1050 Ti，略强于RX560)，GPU1是Intel HD 630
4. onnxruntime版本v1.6.0，ncnn版本tag 20201218，vulkan SDK v1.2.162.0，OpenCv 3.4.13
5. 选取了2张测试图，第一张为分辨率640x640的普通图片，测试100个循环取平均值(对应第一个表格)；第二张为分辨率810x12119的超长图片，测试20个循环取平均值(对应第二个表格)
6. 测试环境：Windows10 x64，macOS 10.15.7
7. 20201129更新：除了推理框架使用OpenMP以外，Demo程序的AngleNet和CrnnNet阶段也使用OpenMP来加速计算。实测OpenMP对DbNet阶段没有加速效果，因为DbNet阶段是单任务单线程的。
8. 20210102更新：ncnn vulkan版，DbNet阶段使用GPU加速，AngleNet和CrnnNet阶段仍然使用CPU+OpenMP多线程计算。

* DbNet阶段特别适合GPU加速，GPU只有compute queue的概念没有线程，低端的GPU只有1个compute queue。
* 在代码改动小的前提下，实测AngleNet和CrnnNet使用CPU多线程计算更快。
* 如果重新设计架构，比如CPU x个线程+GPU 1~N个任务，这样并行计算，理论上行得通，但整体改动很大。
* 因为AngleNet和CrnnNet的数据量小，计算耗时小，即使通过复杂的算法和架构实现提升，也对整体的结果影响小，投入产出比小。
* 这样的架构好处是：GPU和CPU轮流使用，不容易过热并可以让GPU和CPU都保持在高效的工作频率。代码改动小、程序结构简单。

### 20210129测试结果

| 版本     | Win DbNet  | Win总耗时 | mac DbNet | mac总耗时 |
| ------- | ------- | ----: | ----: | ----: |
| onnx(cpu)  | 153.18ms | 216.28ms | 130.38ms | 168.58ms |
| ncnn(cpu)  | 445.01ms | 493.07ms | 223.18ms | 248.06ms |
| ncnn(gpu0) | 78.1ms  | 123.84ms | 35.48ms  | 60.12ms  |
| ncnn(gpu1) | 221.09ms | 274.91ms |  |  |

| 版本     | Win DbNet  | Win总耗时 | mac DbNet | mac总耗时 |
| ------- | ------- | ----: | ----: | ----: |
| onnx(cpu)  | 2166.02ms | 5369.50ms | 1982.50ms | 3760.81ms |
| ncnn(cpu)  | 8360.09ms | 12619.38ms | 4179.19ms | 6567.93ms |
| ncnn(gpu0) | 824.08ms  | 5007.34ms | 601.29ms  | 929.20ms  |
| ncnn(gpu1) | 4197.62ms | 4984.89ms |  |  |

--------

### 20210102测试结果

| 版本     | Win DbNet  | Win总耗时 | mac DbNet | mac总耗时 |
| ------- | ------- | ----: | ----: | ----: |
| onnx(cpu)  | 149.50ms | 201.49ms | 122.17ms | 159.48ms |
| ncnn(cpu)  | 417.87ms | 468.13ms | 202.74ms | 230.80ms |
| ncnn(gpu0) | 75.33ms  | 123.89ms | 36.69ms  | 64.06ms  |
| ncnn(gpu1) | 221.10ms | 356.37ms | 81.41ms  | 109.14ms |

| 版本     | Win DbNet  | Win总耗时 | mac DbNet | mac总耗时 |
| ------- | ------- | ----: | ----: | ----: |
| onnx(cpu)  | 2125.61ms | 5360.59ms | 1897.68ms | 3689.66ms |
| ncnn(cpu)  | 8025.73ms | 12400.37ms | 3713.89ms | 6193.08ms |
| ncnn(gpu0) | 799.64ms  | 5126.97ms | 608.93ms  | 903.53ms  |
| ncnn(gpu1) | 4155.91ms | 4823.95ms | 1757.84ms  | 2046.62ms |

#### 推理速度对比总结：

1. 仅使用CPU计算时，在mac平台上onnxruntime是ncnn速度的1.5~1.7倍，在win平台则是约2倍，结论是cpu计算方面onnxruntime快很多。
2. 使用GPU0加速时，使用AMD三百元级别的低端显卡(性能大概略强于RX560)，DbNet阶段加速约5~10倍。
3. 使用独立显卡GPU0加速时，图片越大，则加速效果越惊人，但奇怪的是，windows下，总耗时数据提升不够大，我个人的猜测是windows下的vulkan仍不够完善，可能从GPU计算完成取出的大量数据送CPU计算的过程造成了延迟，在mac下则表现正常。
4. 使用核芯显卡GPU1加速时，也能获得不错的加速效果，至少也比ncnn(cpu)快了一倍，并且在windows下，总耗时也很正常，在大图时，竟然夺得了测试成绩第一，可能是核芯显卡与CPU集成在一起的，所以数据传输延迟很低。
5. 建议：如果没有任何GPU，且CPU的核心数量很多，则使用onnx版最佳。如果有独立显卡且支持vulkan，则使用ncnn vulkan是最好的选择。如有没有独立显卡但CPU带intel核显，则不妨试试ncnn
   vulkan，性能提升相当于白捡。
6. 20210129更新：20210129的测试结果中的mac gpu1发生段错误，结果暂缺，问题原因待排查。

#### 其它

对于老平台的支持问题(主要指windows平台)：

* opencv通过自行编译，最老可以支持xp。
* ncnn(cpu)通过自行编译，最老也能支持xp。
* onnxruntime只能支持windows7~10。
* 所以想上老windows平台，则只能选择ncnn版。