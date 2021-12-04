#include<iostream>
#include<string.h>
#include<ctime>
#include<fstream>
using namespace std;

int data[34000000];
ofstream outfile("result.txt", ios::trunc); // ����ļ�

long getTimeNs(){ // ��ȡ���뼶ʱ�� 
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

inline unsigned long long currentcycles(){ // ���� cpu ���� 
	
    unsigned lo, hi;
    __asm__ __volatile__("rdtsc": "=a"(lo), "=d"(hi));
    return (unsigned long long)hi << 32 | lo;
    
}

int test(int elems, int stride){ // ���Ժ�����������Ԫ����� 
	
	int i;
	int sx2 = stride * 2, sx3 = stride * 3, sx4 = stride * 4;
	int acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
	int length = elems;
	int limit = length - sx4;
		
	for(i = 0; i < limit; i += sx4){ // 4 * 4 ��ѭ��չ�� 
		acc0 += data[i];
		acc1 += data[i + stride];
		acc2 += data[i + sx2];
		acc3 += data[i + sx3];
	}
	
	for(; i < length; i += stride)
		acc0 += data[i];
		
	return ((acc0 + acc1) + (acc2 + acc3));	
}

void run(int size, int stride){ // ִ�� test ������ͳ������ʱ��� cpu ������ 
	
	int elems = size / sizeof(int);
	
	long start, end;
	unsigned long long t1, t2;
	
	start =  getTimeNs();
	t1 = currentcycles();
	test(elems, stride);	
	t2 = currentcycles();
	end =  getTimeNs();
	
	double cycles = t2 - t1;
	long time = end - start;
	double CPE = elems / stride / cycles;
	double throughput = elems / stride / (double)time * 1000000000 / 250000;
	
	char c = time >= 1000 ? 'u' : 'n';
	int t = time >= 1000 ? time / 1000 : time;
	
	cout << "ʱ�䣺" << t << c << "s" << endl;
	cout << "CPE��" << CPE << endl;
	cout << "��������" << throughput << "MB/s"<< endl; 
	cout << "CPUƵ�ʣ�" << cycles / time  << "GHZ" << endl << endl; 
	 
	outfile << throughput << endl;
}

int main(){
	
	memset(data, 1, sizeof(data));
	
	int step[12], total[18], s = 1024;
	for(int i = 0; i < 12; i++) // ������ 1 �� 12 
		step[i] = (i + 1); 
	for(int i = 0; i < 18; i++){ // �������� 1K ������ 128M 
		total[i] = s;
		s *= 2;
	}		
	
	for(int i = 0; i < 18; i++){
		for(int j = 0; j < 12; j++){
			int t = total[i] / 1024;
			char c = t >= 1024 ? 'M' : 'K';
			t = t >= 1024 ? t / 1024 : t;			
			cout << "��������" << t << c << endl;
			cout << "������" << step[j] << endl;
			
			outfile << i + 1 << " " << j + 1 << " "; 
			
			run(total[i], step[j]);
		}
	}
	
	
	return 0;
}
