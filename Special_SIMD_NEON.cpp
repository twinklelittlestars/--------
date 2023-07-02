#include<iostream>
#include<fstream>
#include<sstream>
#include<stdio.h>
#include<time.h>
#include<arm_neon.h>
struct timespec sts, ets;
const int col = 4000, row = 60000;
using namespace std;
int n, eliminated_row;
struct bitstorage {
	unsigned int bits_array[col];
	int big_value = -1;
	
	void addNum(int num) {
		int n1 = num / 32, n2 = num % 32;
		bits_array[n1] += (0x80000000 >> n2);
		big_value = max(big_value, num);
		n = max(n, n1 + 1);
	}
};

void print(bitstorage& b, ofstream& of) {
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
		uint32x4_t v1 = vld1q_u32(&b1.bits_array[i]);
		uint32x4_t v2 = vld1q_u32(&b2.bits_array[i]);
		uint32x4_t result = veorq_u32(v1, v2);
		vst1q_u32(&b1.bits_array[i], result);
	}
	for (; i <= n; ++i)
		b1.bits_array[i] ^= b2.bits_array[i];
	int k = n, j = 31;
	while (b1.bits_array[k] == 0 && k >= 0)
		--k;
	if (k < 0) {
		b1.big_value = -1;
		return;
	}
	unsigned temp = 1;
	while ((b1.bits_array[k] & temp) == 0 && j >= 0)temp <<= 1, --j;
	b1.big_value = 32 * k + j;

}
bitstorage consumption[row], eliminated[row];
void special_neon_LU() {
	for (int i = 0; i < eliminated_row; ++i) {
		while (eliminated[i].big_value != -1) {
			if (consumption[eliminated[i].big_value].big_value != -1) {
				Xor(eliminated[i], consumption[eliminated[i].big_value]);
			}

			else {
				consumption[eliminated[i].big_value] = eliminated[i];
				break;
			}
		}
	}
}
void timestart()
{
	timespec_get(&sts, TIME_UTC);
}
void timestop()
{
	timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec - sts.tv_sec;
	long dnsec = ets.tv_nsec - sts.tv_nsec;
	if (dnsec < 0)
	{
		dsec--;
		dnsec += 1000000000ll;
	}
	printf("%llu.%09llu\n", dsec, dnsec);
}
void readData() {
	fstream fs1("/home/data/Groebner/5_2362_1226_453/1.txt");
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
	fs1.open("/home/data/Groebner/5_2362_1226_453/2.txt");
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
	special_neon_LU();
	timestop();
}