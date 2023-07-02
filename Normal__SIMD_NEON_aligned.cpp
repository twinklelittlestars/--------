#include <stdio.h>
#include <time.h>
#include <iostream>
#include<ctime>
#include<arm_neon.h>
using namespace std;
float32x4_t par1;
struct timespec sts,ets;
float matix[2048][2048];
void setmatix(int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matix[i][j] = 1.00 / (i + j + 1.00);
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
    time_t dsec=ets.tv_sec-sts.tv_sec;
    long dnsec=ets.tv_nsec-sts.tv_nsec;
    if(dnsec<0)
    {
        dsec--;
        dnsec+=1000000000ll;
    }
    printf ("%llu.%09llu \n",dsec,dnsec);
}
void neon_aligned(int N)
{       int k;
        for ( k = 0; k < N; k++)
        {
            int j;
            int off=((unsigned long int)(&matix[k][k+1]))%16/4;
                for(j=k+1;j<k+1+off&&j<N;j++)
                {
                    matix[k][j]=matix[k][j]/matix[k][k];
                }
                for (; j <= N-4; j+=4)
                {
                        par1=vld1q_f32(&(matix[k][j]));
                        float32x4_t temp=vmovq_n_f32(matix[k][k]);
                        par1=vdivq_f32(par1,temp);
                        vst1q_f32(&(matix[k][j]),par1);

                }
                for(;j<N;j++)
                {
                    matix[k][j]=matix[k][j]/matix[k][k];//����8�е�������
                }
                matix[k][k] = 0;

                for (int i = k + 1; i < N; i++)
                {
                    int j;
                    int off=((unsigned long int)(&matix[i][k+1]))%16/4;//��4�ֽ�Ϊ��λ��ȡ����
                    for(j=k+1;j<k+1+off&&j<N;j++)
                    {
                        matix[i][j]-=matix[i][k]*matix[k][j];
                    }
                    for (;j <= N-4; j+=4)
                    {
                        par1=vld1q_f32((&matix[i][j]));
                        float32x4_t temp1=vmovq_n_f32(matix[i][k]);
                        float32x4_t temp2=vld1q_f32(&(matix[k][j]));
                        temp1=vmulq_f32(temp1,temp2);
                        par1=vsubq_f32(par1,temp1);

                    }
                    for(;j<N;j++)
                    {
                        matix[i][j]-=matix[i][k]*matix[k][j];
                    }
                    matix[i][k] = 0;
                }

        }
}
int main()
{
    setmatix(100);
    timestart();
    neon_aligned(100);
    timestop();

    setmatix(300);
    timestart();
    neon_aligned(300);
    timestop();

    setmatix(600);
    timestart();
    neon_aligned(600);
    timestop();

    setmatix(1024);
    timestart();
    neon_aligned(1024);
    timestop();

    setmatix(1200);
    timestart();
    neon_aligned(1200);
    timestop();

    setmatix(1400);
    timestart();
    neon_aligned(1400);
    timestop();

    setmatix(1600);
    timestart();
    neon_aligned(1600);
    timestop();

    setmatix(1800);
    timestart();
    neon_aligned(1800);
    timestop();

    setmatix(2000);
    timestart();
    neon_aligned(2000);
    timestop();

    setmatix(2024);
    timestart();
    neon_aligned(2024);
    timestop();
    return 0;
}