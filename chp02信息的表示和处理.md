# 第 2 章  信息的表示和处理

## 2.1 信息表示

为了保证程序的可移植性，推荐使用 `int32_t` 、`int64_t`、`uint32_t` 、`uint64_t` 这些指定数据位数的类型，不要使用依赖于编译器、机器字长的数据类型（如 `long` 等）。

**1、指针**

  C 编译器把每个指针和类型信息联系起来，根据指针类型，生成不同的机器码访问指针所指位置的值。
  使用一个字节类型的指针 `typedef unsigned char *byte_pointer`，能够查看一个变量在内存当中的详细存储内容。
  请参见 `字节指针.cpp` 。

```cpp
// 字节指针.cpp
#include<stdio.h>
#include<float.h>

typedef unsigned char *byte_pointer; // 字节指针

// 将start所指向的地址空间的，长度为len的内容，以字节形式输出 
void show_bytes(byte_pointer start, size_t len) {
	for(size_t i = 0; i < len; i++)
		printf("%.2x ", start[i]); // .2x 表示整数必须用至少两个数字的十六进制显示 
	printf("\n");
}

int main(){
	
	short m = -1;
	unsigned short n = m; // 整型间的类型转换，不改变原变量的位值 
	show_bytes((byte_pointer) &m, sizeof(short)); 
	show_bytes((byte_pointer) &n, sizeof(unsigned short));
	printf("m = %d, n = %d\n\n", m, n); 
	
	int x = -1;
	float y = float(x); // 整型到浮点型的强制类型转换，会改变原变量的位值 
	show_bytes((byte_pointer) &x, sizeof(int)); 
	show_bytes((byte_pointer) &y, sizeof(float));
	printf("x = %d, y = %f\n\n", x, y);
	
	float a = -1;
	int b = int(a); // 浮点型到整型的强制类型转换，会改变原变量的位值
	show_bytes((byte_pointer) &a, sizeof(int)); 
	show_bytes((byte_pointer) &b, sizeof(float));
	printf("a = %f, b = %d\n\n", a, b);
	
	float max = FLT_MAX, neg1 = -1; // C++ 支持 IEEE754 浮点标准 
	show_bytes((byte_pointer) &max, sizeof(float)); 
	show_bytes((byte_pointer) &neg1, sizeof(float)); 
	printf("max = %f, -1 = %f\n\n", max, neg1);
	
	
	int undefined = (int)1e10; // 产生结果为 INT_MAX 
	show_bytes((byte_pointer) &undefined, sizeof(int)); 
	printf("undefined = %d", undefined); 
	return 0;
}
```



**2、寻址与字节顺序** 

对于跨越多字节的程序对象，有两个关键问题：

1. 对象的地址是什么？

   - 通常，对象地址为所使用的字节中 **最小的地址** 

2. 在内存中如何排列这些字节？

   - 小端存储：最低有效字节在低地址；最高有效字节在高地址

   - 大端存储：与小端存储相反

     > 小端法：最低有效字节在最前面————目前计算机中使用的方法。
     > 大端法：最高有效字节在最前面————符合阅读习惯。
   
   - example：假设变量 x 的类型为 int，位于地址 0X100 处，它的十六进制值为 0X01234567 。 地址范围 0X100~0X103 的字节顺序依赖于机器的类型：
   
     ![大端小端存储示例](img/fig2_1.png)

**3、不用临时变量实现 swap** 

```c
void inplace_swap(int *x, int *y)
{
    if (x == y)
        return;		// 避免 x 与 y 地址相同时，使内存单元变为 0
    *y = *x ^ *y;
    *x = *x ^ *y;
    *y = *x ^ *y;
}
```



## 2.2 整数表示

- 理解有符号数、无符号数、数据类型表示范围、补码、数字位数扩展（零扩展与符号扩展）、数字截断、类型转换时底层位不变特性。

- 补码最高有效位的权重为 -x<sub>w-1</sub> * 2<sup>w-1</sup>，其余位和原码表示的权值相同。

- 几种常见的数值：

  2<sup>w</sup>：取模时用到，保留 w 位有效值

  2<sup>w</sup> - 1：w 位变量能表示的最大值

  2<sup>w-1</sup>：第 w 位的权重

  2<sup>w-1</sup> - 1：w 位有符号数能表示的最大值

- 有符号与无符号整数转换时，保持位值不变。但浮点数和整数转换时，会根据变量的存储规则改变位值。请参见 `字节指针.cpp`

- **将 short 转换成 unsigned 时，先进行位扩展，再进行符号转换**。unsigned a = short(-1)，得到结果为2<sup>32</sup> - 1。

- 当执行一个运算时，如果一个操作数是有符号的而另一个操作数是无符号的，则 C 语言会 **隐式将有符号数强制类型转换为无符号数** ，并假设这两个数都是非负的，来执行这个运算。 -->  导致对于 `<` 和 `>` 这样的关系型运算符来说，导致非直观的结果。

  example: 

  code 1: 

  ```c
  /* WARNING: This is buggy code */
  float sum_elements(float a[], unsigned length)
  {
      int i;
      float result = 0;
  
      for (i = 0; i <= length - 1; i++)
          result += a[i];
      return result;
  }
  
  // fix 1
  float sum_elements(float a[], unsigned length)
  {
      int i;
      float result = 0;
  
      for (i = 0; i < length; i++)	// change circle stop condition
          result += a[i];
      return result;
  }
  
  // fix 2
  float sum_elements(float a[], int length)	// change type of paramter length
  {
      int i;
      float result = 0;
  
      for (i = 0; i <= length - 1; i++)
          result += a[i];
      return result;
  }
  ```

  code 2：

  ```c
  /* Determine  whether  strings  is  longer  than  string t  */ 
  /*  WARNING:  This  function  is  buggy  */
  int strlonger(char  *s,  char  *t) 
  { 
  	return  strlen(s)  - strlen(t)  >  0; 
  }
  
  // fix
  int strlonger(char  *s,  char  *t) 
  { 
  	return  strlen(s)  > strlen(t); 	// return type of strlen() is size_t 
      									// (size_t 在头文件 stdio.h 中通过 typedef 定义，为 unsigned int/long)
  }
  ```



## 2.3  整数运算

**溢出** ： 指完整的整数运算结果不能放到数据类型的字长限制中去。

- 无符号数加法：溢出时高位截断（相当于取模）

  如何判断两无符号数加法发生溢出？

  > 设操作数为 x 和 y，运算结果为 s。若 s < x 或者 s < y，说明发生溢出。

- 补码加法：溢出时同样高位截断。具体有以下几种情况：

![补码加法](https://user-images.githubusercontent.com/56211928/141666234-c5f8f877-7154-41fd-a311-962ca0825b3c.png)

> 可以这样理解：对于两个正数 x, y 相加，最高位的权重本应为 2^(w-1)，如果发生溢出，最高位权重变成 -2^(w-1)，故实际结果为 x + y - 2^w，负数同理

![](https://user-images.githubusercontent.com/56211928/141666298-6e8bb229-e57a-4f1a-ba59-3635a0c1a010.png)

如何判断发生发生补码加法溢出？

> 设操作数为 x 和 y，运算结果为 s。有以下两种情况：
>
> 1. 当且仅当 x > 0, y > 0，但 s $\leq$ 0 时，发生正溢出。
> 1. 当且仅当 x < 0, y < 0，但 s $\geq$ 0 时，发生负溢出。

- 计算补码的非（可以理解为计算相反数）

  > TMin 的补码非为本身；其余数 x 的补码非为 -x 。

  补码非的位级表示：

  > 1. C 语言内，对任意整数值 x， -x == ~x + 1 （取反加 1 ）永远成立。
  >
  > 2. ~x + 1 的特殊计算方法：找到数字 x 二进制表示从左向右数的第一个 1，然后，将第一个 1 的左侧二进制数全部取反，其余不变，即可得到 -x 。
  >
  >    eg：补码： 11001000 --> 取反： 00111000

- 对于整数乘法，C 语言编译器可以利用移位、加法、减法的组合来消除乘以常数的情况。

- 对于整数除法（除以 2 的幂），直接移位导致向下舍入。可以利用 (x + (1 << k)) >> k 得到向上舍入（对于负数应该向上舍入）。

  code：

  ```cpp
  #include <iostream>
  using namespace std;
  
  int main()
  {
      int a = -125;
      int k = 1;
      int b = a >> k;                  // 向下舍入(取整)
      int c = a / 2;                   // 除法
      int d = (a + (1 << k) - 1) >> k; // 向上舍入(取整)(仅适用于 a < 0 时)
      /*
      	表达式 (x < 0 ? x + (1 << k) - 1 : x) >> k 对任意有符号数 x ，计算数值 x/(2^k)
      */
      cout << a << "  " << b << "  " << c << "  " << d << endl;
      // 输出：-125  -63  -62  -62
      return 0;
  }
  ```

  


## 2.4  浮点数

- IEEE 浮点标准用 V = (-1)<sup>s</sup> * M * 2<sup>E</sup> 的形式表示一个浮点数

  > s 为符号位，1 为负数，0 为正数
  > E 为阶码，用 k 位字段 exp 进行编码
  > M 为尾数，用 n 位字段 frac 编码

  C 语言中，float 类型 s 、M 、E 位数分别为 1、23、8；double 类型分布为 1、52、11 。

  ![](https://user-images.githubusercontent.com/56211928/141666981-59757bf1-f902-44e2-9ae8-541719c58cd7.png)

- 根据阶码 exp 的值，可将浮点数分为以下几种情况：

  - 规格化值：

    > exp 的位既不全 0 也不全 1
    > 阶码 E = exp - Bias （单精度位 255，双精度为 2047）。
    > 尾数 M = 1 + frac

  - 非规格化值：提供一种 0 的表示方法，还可以表示一些非常接近 0.0 的数

    > exp 全为 0
    > 阶码 E = 1 - Bias （如此定义可实现最小规格化数和最大非规格化数的平滑过渡）
    > 尾数 M = frac

  - 特殊值：

    > exp 全 1。小数域全 0 时，根据符号表示正负无穷；小数域为非零时，表示 NaN。

- 1 位符号，4 位阶码（Bias = 7），3 位尾数所表示的浮点值如下图：      
  ![image](https://user-images.githubusercontent.com/56211928/141667138-73d8c8de-1d3b-4286-b664-85bf5ca4add1.png)

- 四种舍入方式

  > 1. 向偶数舍入：又称向最邻近的值舍入，确定两个可能结果 **中间值** 的舍入效果，使得最低有效数字是偶数（二进制最低有效位为 0）。避免计算平均值代来的系统误差。
  >        eg（舍入到 1/4）：10.00110 -> 10.01    10.11100 -> 11.00    10.10100 -> 10.10
  > 2. 向零舍入：直接丢弃被舍去部分
  > 3. 向下舍入：
  > 4. 向上舍入：

- 浮点运算特性：浮点运算可交换；**浮点加法和乘法都不满足结合性** （舍入带来的精度问题）；浮点乘法在加法上不具备分配属性。

- C 语言中的浮点数：符合 IEEE 754 标准，请参见 `字节指针.cpp` 。float 类型一些特殊值表示如下：

  ![单精度浮点数各种极值情况 picture from: https://zh.wikipedia.org/wiki/IEEE_754#32%E4%BD%8D%E5%96%AE%E7%B2%BE%E5%BA%A6](img/fig2_2.png)



## 2.5 部分习题

**习题 2.65** 

```c
/** 
 * Return 1 when x contains an odd number of 1; 0 otherwise.
 */
int odd_ones(unsigned x)
{
    // assume that width of x is 32 bits
    x ^= x >> 16;	// 相当于把 x 高 16 位与 x 低 16 位做异或运算，
    				// 这样，所得结果高 16 位都变为0；
    				// 低 16 位对应位置若相同，经过异或运算变为 0（相当于去掉偶数个 1）；若不同，结果为 1
    				// 然后，对低 16 位重复类似操作，直到只剩 1 位。
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}
```



