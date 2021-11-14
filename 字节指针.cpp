#include<stdio.h>
#include<float.h>

typedef unsigned char *byte_pointer; // 字节指针

// 将start所指向的地址空间的，长度为len的内容，以字节形式输出 
void show_bytes(byte_pointer start, size_t len) {
	for(size_t i = 0; i < len; i++)
		printf("%.2x ", start[i]); // .2x表示整数必须用至少两个数字的十六进制显示 
	printf("\n");
}

int main(){
	
	short m = -1;
	unsigned short n = m; // 整形间的类型转换，不改变原变量的位置 
	show_bytes((byte_pointer) &m, sizeof(short)); 
	show_bytes((byte_pointer) &n, sizeof(unsigned short));
	printf("m = %d, n = %d\n\n", m, n); 
	
	int x = -1;
	float y = float(x); // 整形到浮点型的强制类型转换，会改变原变量的位值 
	show_bytes((byte_pointer) &x, sizeof(int)); 
	show_bytes((byte_pointer) &y, sizeof(float));
	printf("x = %d, y = %f\n\n", x, y);
	
	float a = -1;
	int b = int(a); // 浮点型到整形的强制类型转换，会改变原变量的位值
	show_bytes((byte_pointer) &a, sizeof(int)); 
	show_bytes((byte_pointer) &b, sizeof(float));
	printf("a = %f, b = %d\n\n", a, b);
	
	float max = FLT_MAX, neg1 = -1; // C++支持IEEE754浮点标准 
	show_bytes((byte_pointer) &max, sizeof(float)); 
	show_bytes((byte_pointer) &neg1, sizeof(float)); 
	printf("max = %f, -1 = %f\n\n", max, neg1);
	
	
	int undefined = (int)1e10; // 产生结果为INT_MAX 
	show_bytes((byte_pointer) &undefined, sizeof(int)); 
	printf("undefined = %d", undefined); 
	return 0;
}
