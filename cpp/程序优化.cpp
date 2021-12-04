#include<iostream>
#include<fstream>
using namespace std;

typedef unsigned long long cycles_t;
typedef int data_t; 
#define OP *
#define IDENT 1

cycles_t t1, t2;
inline cycles_t currentcycles(){ // ��¼ cpu ʱ�������� 
	
    cycles_t result;
    __asm__ __volatile__ ("rdtsc" : "=A" (result));
        
    return result;
}

typedef struct{ // ����ṹ�� 
	int len;
	data_t *data;
}vec_rec, *vec_ptr;

vec_ptr new_vec(long len){ // �������� 

	vec_ptr result = (vec_ptr)malloc(sizeof(vec_rec));
	data_t *data = NULL;
	if(!result)
		return NULL;
	result->len = len;
	
	if(len > 0){
		data = (data_t *)calloc(len, sizeof(data_t));
		if(!data){
			free((void *)result);
			return NULL;
		}	
	}
	
	result->data = data;
	return result;
} 

int get_vec_element(vec_ptr v, int index, data_t *dest){ // ��ȡ��ֵ�е�ĳԪ�� 
	
	if(index < 0 || index >= v->len)
		return 0;	
	*dest = v->data[index];

	return 1; 
} 

int vec_length(vec_ptr v){ // ��ȡ���鳤�� 
	return v->len;
}

data_t *get_vec_start(vec_ptr v){ // ��ȡ�����׵�ַ 
	return v->data;
}

vec_ptr initial(int n){ // �����ʼ�� v[i] = i + 1 

	vec_ptr v = new_vec(n);
	for(int i = 0; i < n; i++)
		v->data[i] = i + 1;
	return v;	
}

void combine1(vec_ptr v, data_t *dest){ // ���κ��Ż� 
	
	*dest = IDENT;
	
	for(int i = 0; i < vec_length(v); i++){
		data_t val;
		get_vec_element(v, i, &val);
		*dest = *dest OP val;
	} 
}

void combine2(vec_ptr v, data_t *dest){ // �ƶ� vec_length 
	
	*dest = IDENT;	
	int length = vec_length(v);
	
	for(int i = 0; i < length; i++){
		data_t val;
		get_vec_element(v, i, &val);
		*dest = *dest OP val;
	} 
}

void combine3(vec_ptr v, data_t *dest){ // ֱ�ӷ������� 
	
	*dest = IDENT;	
	int length = vec_length(v);
	data_t *data = get_vec_start(v); 
	
	for(int i = 0; i < length; i++)
		*dest = *dest OP data[i];
			
}

void combine4(vec_ptr v, data_t *dest){ // �ۻ�����ʱ������ 
	
	data_t acc = IDENT;	
	int length = vec_length(v);
	data_t *data = get_vec_start(v); 
	
	for(int i = 0; i < length; i++)
		acc = acc OP data[i];
	
	*dest = acc;			
}

void combine5(vec_ptr v, data_t *dest){ // 5 * 1 չ�� 
	
	int i; 
	data_t acc = IDENT;	
	int length = vec_length(v);
	int limit = length - 4;
	data_t *data = get_vec_start(v); 
	
	for(i = 0; i < limit; i += 5)
		acc = ((((acc OP data[i]) OP data[i + 1]) OP data[i + 2]) OP data[i + 3]) OP data[i +4];
	for(; i < length; i++)
		acc = acc OP data[i];
		
	*dest = acc;
					
}

void combine6(vec_ptr v, data_t *dest){ // 5 * 5 չ�� 
	
	int i; 
	data_t acc1 = IDENT;
	data_t acc2 = IDENT;
	data_t acc3 = IDENT;
	data_t acc4 = IDENT;
	data_t acc5 = IDENT;	
	int length = vec_length(v);
	int limit = length - 4;
	data_t *data = get_vec_start(v); 
	
	for(i = 0; i < limit; i += 5){
		acc1 = acc1 OP data[i];
		acc2 = acc2 OP data[i + 1];
		acc3 = acc3 OP data[i + 2];
		acc4 = acc4 OP data[i + 3];
		acc5 = acc5 OP data[i + 4];
	}

	for(; i < length; i++)
		acc1 = acc1 OP data[i];
		
	*dest = acc1 OP acc2 OP acc3 OP acc4 OP acc5;
					
}

void combine7(vec_ptr v, data_t *dest){ // 5 * 1a չ�� 
	
	int i; 
	data_t acc = IDENT;	
	int length = vec_length(v);
	int limit = length - 4;
	data_t *data = get_vec_start(v); 
	
	for(i = 0; i < limit; i += 5)
		acc = acc OP (data[i] OP (data[i + 1] OP (data[i + 2] OP (data[i + 3] OP data[i +4]))));
	for(; i < length; i++)
		acc = acc OP data[i];
		
	*dest = acc;
					
}

int main(){	
	
	ofstream outfile("result5.txt", ios::trunc); // ����ļ� 
		
	data_t dest;
	for(int i = 0; i < 1000; i++){
		for(int j = 50; j < 200; j++){
			vec_ptr v = initial(j);
			
			cycles_t t1 = currentcycles();
			combine5(v, &dest); // ѡ��ͬ�ĺ��������� CPE 
			cycles_t t2 = currentcycles();
			
			outfile << j << " " << t2 - t1 << endl;
			// cout << dest << endl;
		}
	}		
			
    return 0;
}
