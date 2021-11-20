# 第 2 章  信息的表示和处理

## 2.1 信息表示

为了保证程序的可移植性，推荐使用 `int32_t` 、`int64_t`、`uint32_t` 、`uint64_t` 这些指定数据位数的类型，不要使用依赖于编译器、机器字长的数据类型（如 `long` 等）。



**寻址与字节顺序** 

对于跨越多字节的程序对象，有两个关键问题：

1. 对象的地址是什么？

   - 通常，对象地址为所使用的字节中 **最小的地址** 

2. 在内存中如何排列这些字节？

   - 小端存储：最低有效字节在低地址；最高有效字节在高地址

   - 大端存储：与小端存储相反

   - example：假设变量 x 的类型为 int，位于地址 0X100 处，它的十六进制值为 0X01234567 。 地址范围 0X100~0X103 的字节顺序依赖于机器的类型：

     ![大端小端存储示例](img/fig2_1.png)

**不用临时变量实现 swap** 

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

理解有符号数、无符号数、数据类型表示范围、补码、数字位数扩展（零扩展与符号扩展）、数字截断、类型转换时底层位不变特性。

注意 C 语言本身的语言特性：

- 当执行一个运算时，如果一个操作数是有符号的而另一个操作数是无符号的，则 C 语言会 **隐式将有符号数强制类型转换为无符号数** ，并假设这两个数都是非负的，来执行这个运算。 -->  导致对于 `<` 和 `>` 这样的关系型运算符来说，导致非直观的结果。

- 避免踩坑：

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
  /* Determine  whether  string s  is  longer  than  string t  */ 
  /*  WARNING:  This  function  is  buggy  */
  int strlonger(char *s,  char *t) 
  { 
  	return  strlen(s) - strlen(t)  >  0; 
  }
  
  // fix
  int strlonger(char  *s,  char  *t) 
  { 
  	return  strlen(s)  > strlen(t);     // return type of strlen() is size_t 
                                          // (size_t 在头文件 stdio.h 中通过 typedef 定义，为 unsigned int/long)
  }
  ```



## 2.3  整数运算

**溢出** ： 指完整的整数运算结果不能放到数据类型的字长限制中去。

1. 无符号数加法：溢出时高位截断（相当于取模）

   如何判断两无符号数加法发生溢出？

   > 设操作数为 x 和 y，运算结果为 s。若 s < x 或者 s < y，说明发生溢出。

2. 补码加法：溢出时同样高位截断。具体有以下几种情况：

   > 1. 正溢出：运算结果超过补码能够表示的最大数
   > 2. 正常
   > 3. 负溢出：运算结果小于补码能表示的最小数

   如何判断发生发生补码加法溢出？

   > 设操作数为 x 和 y，运算结果为 s。有以下两种情况：
   >
   > 1. 当且仅当 x > 0, y > 0，但 s $\leq$ 0 时，发生正溢出。
   > 1. 当且仅当 x < 0, y < 0，但 s $\geq$ 0 时，发生负溢出。
   
3. 补码的非：

   > TMin 的补码非为本身；其余数 x 的补码非为 -x 。

   补码非的位级表示：

   > 1. C 语言内，对任意整数值 x， -x == ~x + 1 永远成立。
   > 2. ~x + 1 的特殊计算方法：找到数字 x 二进制表示从左向右数的第一个 1，然后，将第一个 1 的左侧二进制数全部取反，其余不变，即可得到 -x 。

4. 无符号乘法：溢出时高位截断（相当于取模）

5. 补码乘法：溢出时高位截断。无符号乘法和补码乘法运算位级等价。

6. 乘以常数：转化为移位运算

7. 除以 2 的幂：

   注意补码除法转换为移位时的取整问题：

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



## 2.4 浮点数

IEEE 浮点表示：
$$
V = {-1}^{s} \times M \times {2}^{E}
$$
其中：

- s：符号
- M：二进制小数，表示尾数
- E：阶码

C 语言中，float 类型 s 、M 、E 位数分别为 1、23、8；double 类型分布为 1、52、11 。

根据阶码（exp）的值，可将浮点数分为以下几种情况：

1. 规格化的值：exp 位模式既不全 0 也不全 1
2. 非规格化的值：exp 全 0
3. 特殊值：exp 全为 1 。小数域全 0 时，根据符号表示正负无穷；小数域为非零时，表示 NaN。



四种舍入方式：

1. 向偶数舍入：又称向最邻近的值舍入，当值等于 xxx.5 时，向偶数舍入
2. 向零舍入：去掉小数
3. 向下舍入：
4. 向上舍入：



浮点运算可交换、不可结合（舍入带来的精度问题）；浮点乘法在加法上不具备分配属性。



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






