# RISCV_pipeline

## 五级流水

- 自测样例已通过
- OJ已通过
- 乱序自测已完成
- 乱序OJ测试已完成

## 文件结构
### RISCV.cpp

### global.hpp 
- 全局变量:PC,mem,reg,clk,eesc
- enum类型: OPflag , object_num
- print函数 

### my_cpu.hpp
- cpu类

### buffer_stages.hpp
- buffer定义
- stages定义
- 五级流水相关代码

### predictor.hpp
1. predictor:

    1. 两位饱和计数器预测:
        - 一级预测
        - 二级自适应预测

| 数据点 | 成功率 | 预测成功次数 | 预测总次数 |
| :----: | :----: | :----: | :----: |
|array_test1|0.500000|suc : 11|cnt 22|

|array_test2|0.461538|suc : 12|cnt 26|

|basicopt1|0.992181|suc : 153926|cnt 155139|

|bulgarian|0.946302|suc : 67654|cnt 71493|

|expr|0.819820|suc : 91|cnt 111|

|gcd|0.633333|suc : 76|cnt 120|

|hanoi|0.990491|suc : 17291|cnt 17457|

|lvalue2|0.850978|suc : 57755|cnt 67869|

|manyarguments|0.800000|suc : 8|cnt 10|

|multiarray|0.549383|suc : 89|cnt 162|

|naive|\|0|0|

|pi|0.844478|suc : 33742289|cnt 39956380|

|qsort|0.967642|suc : 193572|cnt 200045|

|queens|0.810571|suc : 62508|cnt 77116|

|statement_test|0.628713|suc : 127|cnt 202|

|superloop|0.992541|suc : 431782|cnt 435027|

|tak|0.805290|suc : 48832|cnt 60639|