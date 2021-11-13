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

