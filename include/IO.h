/* File:     IO.h
 * Purpose:  Header file for IO.c.
 */
#ifndef _MY_IO_H_
#define _MY_IO_H_

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

int ReadFilePara(char path[], int *profilenum, int *genelen, int *LineLength);
int ReadFile(char path[],int LineLength,int BeginLine,int EndLine,int profilenum, int genelen, short **profileSet);
void WritetxtResult(int sourceBegin ,int sourceEnd, int matlen, char writepath[], float **ES_Matrix);
void getGeneSet(short gs[],int *count, char gsStr[]);
int ReadMatrixFilePara(char path[], int *profilenum1, int *profilenum2, int *LineLength);
int ReadMatrixFile(char path[],int LineLength,int BeginLine,int EndLine,int profilenum1, int profilenum2, float **Matrix);
void WritetxtClusterResult(int classflag[] ,int len, char writepath[]);

#endif
