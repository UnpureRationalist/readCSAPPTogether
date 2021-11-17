#include<stdio.h>
#include<float.h>

typedef unsigned char *byte_pointer; // �ֽ�ָ��

// ��start��ָ��ĵ�ַ�ռ�ģ�����Ϊlen�����ݣ����ֽ���ʽ��� 
void show_bytes(byte_pointer start, size_t len) {
	for(size_t i = 0; i < len; i++)
		printf("%.2x ", start[i]); // .2x��ʾ���������������������ֵ�ʮ��������ʾ 
	printf("\n");
}

int main(){
	
	short m = -1;
	unsigned short n = m; // ���μ������ת�������ı�ԭ������λֵ 
	show_bytes((byte_pointer) &m, sizeof(short)); 
	show_bytes((byte_pointer) &n, sizeof(unsigned short));
	printf("m = %d, n = %d\n\n", m, n); 
	
	int x = -1;
	float y = float(x); // ���ε������͵�ǿ������ת������ı�ԭ������λֵ 
	show_bytes((byte_pointer) &x, sizeof(int)); 
	show_bytes((byte_pointer) &y, sizeof(float));
	printf("x = %d, y = %f\n\n", x, y);
	
	float a = -1;
	int b = int(a); // �����͵����ε�ǿ������ת������ı�ԭ������λֵ
	show_bytes((byte_pointer) &a, sizeof(int)); 
	show_bytes((byte_pointer) &b, sizeof(float));
	printf("a = %f, b = %d\n\n", a, b);
	
	float max = FLT_MAX, neg1 = -1; // C++֧��IEEE754�����׼ 
	show_bytes((byte_pointer) &max, sizeof(float)); 
	show_bytes((byte_pointer) &neg1, sizeof(float)); 
	printf("max = %f, -1 = %f\n\n", max, neg1);
	
	
	int undefined = (int)1e10; // �������ΪINT_MAX 
	show_bytes((byte_pointer) &undefined, sizeof(int)); 
	printf("undefined = %d", undefined); 
	return 0;
}
