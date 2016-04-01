#include "stdio.h"
#include "stdlib.h"
#include <string.h> 
#include <math.h> 
#include "time.h"
#include <sys/time.h> 
#include "RandomChange.h"

//��1-total��������������size�����ظ������
void GetRandomSequence(int total,int size , float* Seq)
{
	int i;
	int *sequence = (int *)malloc(total*sizeof(int));

    for (i = 0; i < total; i++)
	{
        sequence[i] = i+1;
    }
	
	struct timeval seed;
	gettimeofday(&seed, NULL);
	srand((unsigned)seed.tv_usec);  //������������������������Ҫ��΢�뼶ʱ��������
    int end = total-1;

    for (i = 0; i < size; i++) 
	{
        int num = rand()%(end+1);
        Seq[i] = (float)sequence[num];
        sequence[num] = sequence[end];
        end--;
    }
	free(sequence);
}

//�����������postions
void changePosition(float *positions , int size) 
{   
	struct timeval seed;
	gettimeofday(&seed, NULL);
	srand((unsigned)seed.tv_usec);  //������������Ҷ�Σ�Ҫ��΢�뼶ʱ��������
	
	int index,rd;
	float temp;
    for( index=size-1; index>=size/2; index--) {
		rd = rand()%(index+1);
		temp = positions[rd];
		positions[rd] = positions[index];   
        positions[index] = temp;   
	}     
}   