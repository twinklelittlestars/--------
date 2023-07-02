#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include <windows.h>
long long head, tail, freq;
const int col = 4000, row = 60000;
using namespace std;
int n, eliminated_row;
struct bitstorage {
	unsigned int bits_array[col];//�洢�����Ƽ����е�ʵ������
	int big_value = -1;//��¼�����Ƽ����е������ֵ
	//����ʵ�ַ�ʽ�ǽ����� num ת��Ϊ�����ƺ󣬽���Ӧ��λ�ϵ�ֵ����Ϊ 1��
	void addNum(int num) {//������bitset������һ������
		int n1 = num / 32, n2 = num % 32;//n1 �� n2 �ֱ��ʾ num ��Ӧ���޷����������� bits �е������Ͷ�Ӧ�Ķ�����λ��
		bits_array[n1] += (0x80000000 >> n2);//
		big_value = max(big_value, num);
		n = max(n, n1 + 1);//��¼�ü�����ʹ�õ�����������λ��
	}
};

void print(bitstorage& b, ofstream& of) {//�� bitset �е�����������ļ���
	if (b.big_value == -1) {
		of << endl;
		return;
	}
	for (int i = n; i >= 0; --i) {
		for (unsigned int temp = 1, j = 31; temp != 0; temp <<= 1, --j)
			if (temp & b.bits_array[i])
				of << i * 32 + j << " ";
	}
	of << endl;
}

void Xor(bitstorage& b1, bitstorage& b2) {
	int i = 0;
	for (; i + 4 <= n; i += 4)
	{
		__m128i v1 = _mm_loadu_si128((__m128i*) & b1.bits_array[i]);
		__m128i v2 = _mm_loadu_si128((__m128i*) & b2.bits_array[i]);
		__m128i result = _mm_xor_si128(v1, v2);
		_mm_storeu_si128((__m128i*) & b1.bits_array[i], result);
	}
	for (; i <= n; ++i)b1.bits_array[i] ^= b2.bits_array[i];
	int k = n, j = 31;
	while (b1.bits_array[k] == 0 && k >= 0)--k;
	if (k < 0) {
		b1.big_value = -1;
		return;
	}
	unsigned temp = 1;
	while ((b1.bits_array[k] & temp) == 0 && j >= 0)temp <<= 1, --j;
	b1.big_value = 32 * k + j;

}
bitstorage consumption[row], eliminated[row];//RΪ��Ԫ�ӣ�EΪ����Ԫ��
void special_sse_LU() {
	for (int i = 0; i < eliminated_row; ++i) {
		while (eliminated[i].big_value != -1) {//�жϱ���Ԫ���Ƿ�ȫ0
			if (consumption[eliminated[i].big_value].big_value != -1) {//�ж���Ԫ���Ƿ��������Ԫ��Ӧ�еı���Ԫ�е���
				Xor(eliminated[i], consumption[eliminated[i].big_value]);//���������н������
			}

			else {
				consumption[eliminated[i].big_value] = eliminated[i];//����������һ�еı���Ԫ�д������Ԫ����
				break;
			}
		}
	}
}
void timestart()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);
}
void timestop()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);	// end time
	cout << ((tail - head) * 1000.0 / freq) << endl;
}
void readData() {//�������ڶ�ȡ�������ݣ����ļ��ж�ȡ���洢�� bitset ��
	fstream fs1("E:\\���г���\\data\\Groebner\\��������1 ��������130��������Ԫ��22������Ԫ��8\\��Ԫ��.txt");
	string line;
	while (getline(fs1, line)) {
		stringstream ss(line);
		int index, num;
		ss >> index;
		consumption[index].addNum(index);
		while (ss >> num)
			consumption[index].addNum(num);
	}
	fs1.close();
	fs1.open("E:\\���г���\\data\\Groebner\\��������1 ��������130��������Ԫ��22������Ԫ��8\\����Ԫ��.txt");
	while (getline(fs1, line)) {
		stringstream ss(line);
		int num;
		while (ss >> num)
			eliminated[eliminated_row].addNum(num);
		++eliminated_row;
	}
	fs1.close();
}
int main() {
	readData();
	timestart();
	special_sse_LU();
	timestop();

	ofstream of("E:\\���г���\\data\\Groebner\\��������1 ��������130��������Ԫ��22������Ԫ��8\\output.txt");
	for (int i = 0; i < eliminated_row; ++i)
		print(eliminated[i], of);
	of.close();
}