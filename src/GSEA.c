#include "stdio.h"
#include "stdlib.h"
#include <string.h> 
#include <math.h> 
#include "time.h"
#include "GSEA.h"
#include "RandomChange.h"

//quicksort
void quiksortINT(int a[],int low,int high)
{
    int i = low;
    int j = high;  
    int temp = a[i]; 
  
    if( low < high)
    {          
        while(i < j) 
        {
            while((a[j] >= temp) && (i < j)) 
                j--; 
            a[i] = a[j];
            while((a[i] <= temp) && (i < j))
                i++;  
            a[j]= a[i];
        }
        a[i] = temp;
        quiksortINT(a,low,i-1);
        quiksortINT(a,j+1,high);
    }
    else
    {
        return;
    }
}

//quicksort
void quiksort(float a[],int low,int high)
{
    int i = low;
    int j = high;  
    float temp = a[i]; 
  
    if( low < high)
    {          
        while(i < j) 
        {
            while((a[j] >= temp) && (i < j)) 
                j--; 
            a[i] = a[j];
            while((a[i] <= temp) && (i < j))
                i++;  
            a[j]= a[i];
        }
        a[i] = temp;
        quiksort(a,low,i-1);
        quiksort(a,j+1,high);
    }
    else
    {
        return;
    }
}

//quicksort the ES for GSEA strucrure
void quiksort_gsea(struct GSEA_RESULT gsea[],int low,int high)
{
    int i = low;
    int j = high;  
    struct GSEA_RESULT temp = gsea[i]; 
  
    if( low < high)
    {          
        while(i < j) 
        {
            while((gsea[j].ES >= temp.ES) && (i < j)) 
                j--; 
            gsea[i] = gsea[j];
            while((gsea[i].ES <= temp.ES) && (i < j))
                i++;
			gsea[j] = gsea[i];
        }
        gsea[i] = temp;
        quiksort_gsea(gsea,low,i-1);
        quiksort_gsea(gsea,j+1,high);
    }
    else
    {
        return;
    }
}

//compute the ES by gene signature hitting vector
float quickGeneSet(float isgs[], int len,int sig)
{	
	int hitsum = sig;
	int misssum = len - sig;
	float max = 0;	  
	float tmp_max = 0;
	float ES = 0;
    
	int i,j;

	quiksort(isgs,0,sig-1); //sort the hit position
	
//	for( i=0 ; i<sig ; i++)
//		printf("%f\t",isgs[i]);
//	printf("\n");
	
	for(i=0; i < sig; i++)
	{	
		if(i==0)
		{  //first hit
			if(isgs[i]>=1)
			{ //not first gene��whether prev is lowest 
				tmp_max = isgs[0]/misssum;
				if(tmp_max > max)
				{
					max	=	tmp_max;
					ES	=	-isgs[0]/misssum;
				}	
			}
			if(isgs[i+1]!=isgs[i]+1)
			{ //later miss��whether current is highest 
				tmp_max = fabs((float)(i+1)/hitsum-(isgs[i]-i)/misssum);
				if(tmp_max > max)
				{
					max	=	tmp_max;
					ES	=	(float)(i+1)/hitsum-(isgs[i]-i)/misssum;
				}	
			}
		}else if(i==len-1)
		{ //final hit, whether current is highest
			tmp_max = fabs((float)(i+1)/hitsum-(isgs[i]-i)/misssum);
			if(tmp_max > max)
			{
				max	=	tmp_max;
				ES	=	(float)(i+1)/hitsum-(isgs[i]-i)/misssum;
			}
			if(isgs[i-1]!=isgs[i]-1)
			{ //prev miss��whether prev is lowest
				tmp_max = fabs((float)i/hitsum-(isgs[i]-i)/misssum);
				if(tmp_max > max){
					max	=	tmp_max;
					ES	=	(float)i/hitsum-(isgs[i]-i)/misssum;
				}	
			}
		}
		else
		{ //middle hit
			if(isgs[i-1]!=isgs[i]-1)
			{ //prev miss��whether prev is lowest
				tmp_max = fabs((float)i/hitsum-(isgs[i]-i)/misssum);
				if(tmp_max > max)
				{
					max	=	tmp_max;
					ES	=	(float)i/hitsum-(isgs[i]-i)/misssum;
				}	
			}
			if(isgs[i+1]!=isgs[i]+1)
			{ //later miss��whether current is highest
				tmp_max = fabs((float)(i+1)/hitsum-(isgs[i]-i)/misssum);
				if(tmp_max > max)
				{
					max	=	tmp_max;
					ES	=	(float)(i+1)/hitsum-(isgs[i]-i)/misssum;
				}	
			}
		}			
	}
	return ES;
}

//get the profile index
void getIndex(short s[],short indexS[],int len)
{
	int i;
	for( i=0 ; i<len; i++)
		indexS[s[i]-1] = i;	
}

//computing the ES of GeneSet-profile
float ES_GeneSet(short gs[], short indexS[],int len,int sig)
{
	int i;
	float isgs[MAX_GENESET] ; 
	
	//get the gene signature hitting vector 
	for( i=0 ; i<sig; i++)
		isgs[i] = indexS[gs[i]-1];
	
	return quickGeneSet(isgs, len, sig);			
}

//compute the Global ES
void getGlobalES(int len,int sig,float global_ES[])
{
	int i;
	float *seq = (float*)malloc(sig*sizeof(float));
	
	for(i=0; i<Global_ES_SIZE; i++)
	{
		GetRandomSequence(len, sig, seq);
		global_ES[i] = quickGeneSet( seq, len, sig);
	}
	free(seq);
}

//GSEA fuction using in quick_search
void GSEA(short gs[], short indexS[],int len,int sig, float *ES, float *NES, double *pv, float global_ES[])
{
	float sum = 0, mean = 0;
	int count = 0;
	int i;
	
	//compute the ES
	*ES = ES_GeneSet(gs, indexS, len, sig);	
	
	//permutate global ES
	changePosition(global_ES, Global_ES_SIZE);

/*	
	for(i=0;i < permutation_SIZE; i++)
		printf("%f\t",global_ES[i]);
	printf("\n");
*/
	//compute the NES,PV
	if(*ES < 0)
	{
		for(i=0;i < permutation_SIZE; i++)
		{
			if( global_ES[i] < *ES )
				sum += global_ES[i];
			if( global_ES[i] < 0)
			{
				mean += global_ES[i];
				count++;
			}
		}
	}
	else
	{
		for(i=0;i < permutation_SIZE; i++)
		{
			if( global_ES[i] > *ES )
				sum += global_ES[i];
			if( global_ES[i] > 0)
			{
				mean += global_ES[i];
				count++;
			}
		}
	}
	*pv = (double)sum/permutation_SIZE;
	mean /= count;
	*NES = *ES/fabs(mean);
}


//original version of computing the ES of profile-pair by computing ES directly without quickES
float ES_Profile_original(short s1[], short s2[],int len,int sig)
{
	int isgsUp[MAX_GENE] ;  
	int isgsDown[MAX_GENE] ;

	float ScorehitUp[MAX_GENE] ;
	float ScoremissUp[MAX_GENE]  ;
	float ScorehitDown[MAX_GENE] ;
	float ScoremissDown[MAX_GENE] ;

	int indexS[MAX_GENE] ;
	
	int flagUp=0,flagDown=0;
	int hitsumUp=sig,hitsumDown=sig;
	float maxUp,maxDown;
	int indexUp,indexDown;
	float ES[2];
        
	int i,j;
	
	for( i=0; i<2; i++){		
		
		memset(isgsUp, 0, len * sizeof(int) );
		memset(isgsDown, 0, len * sizeof(int) );  
		
		if(i==0){ 
			//index for s2
			for( j=0 ; j<len; j++)
				indexS[s2[j]] = j;
			//compute isgsUp and isgsDown
			for( j=0 ; j<sig; j++)
			{
				isgsUp[indexS[s1[j]]] = 1;
				isgsDown[indexS[s1[len-j-1]]] = 1;
			}				
		}else{
			//index for s1
			for( j=0 ; j<len; j++)
				indexS[s1[j]] = j;
			//compute isgsUp and isgsDown
			for( j=0 ; j<sig; j++)
			{
				isgsUp[indexS[s2[j]]] = 1;
				isgsDown[indexS[s2[len-j-1]]] = 1;
			}				
		}

		ScorehitUp[0]=isgsUp[0];
		ScoremissUp[0]=1-isgsUp[0];
		ScorehitDown[0]=isgsDown[0];
		ScoremissDown[0]=1-isgsDown[0];
		maxUp=fabs(ScorehitUp[0]/hitsumUp-ScoremissUp[0]/(len-hitsumUp));
		indexUp=0;
		maxDown=fabs(ScorehitDown[0]/hitsumDown-ScoremissDown[0]/(len-hitsumDown));
		indexDown=0;
			
		for( j=1;j<len;j++){
			
			//compute up gene
			ScorehitUp[j] = isgsUp[j]+ScorehitUp[j-1];
			ScoremissUp[j] = (1-isgsUp[j])+ScoremissUp[j-1]; //ǰ���ۼ�
			if(fabs(ScorehitUp[j]/hitsumUp-ScoremissUp[j]/(len-hitsumUp))>maxUp){
				maxUp	=	fabs(ScorehitUp[j]/hitsumUp-ScoremissUp[j]/(len-hitsumUp));
				indexUp	=	j;
			}
				
			//compute down gene
			ScorehitDown[j] = isgsDown[j]+ScorehitDown[j-1];
			ScoremissDown[j] = (1-isgsDown[j])+ScoremissDown[j-1];
			if(fabs(ScorehitDown[j]/hitsumDown-ScoremissDown[j]/(len-hitsumDown))>maxDown){
				maxDown	= fabs(ScorehitDown[j]/hitsumDown-ScoremissDown[j]/(len-hitsumDown));
				indexDown	=	j;
			}
		}

		float ESUp = ScorehitUp[indexUp]/hitsumUp-ScoremissUp[indexUp]/(len-hitsumUp);
		float ESDown = ScorehitDown[indexDown]/hitsumDown-ScoremissDown[indexDown]/(len-hitsumDown);
			
		ES[i] =  ( ESUp - ESDown )/2;
	}

	return (ES[0]+ES[1])/2;
}

//compute the ES of profile-pair
float ES_Profile(short s1[], short s2[],int len,int sig)
{
	int i;
	short *gsUp = (short *)malloc(sig*sizeof(short));
	short *gsDown = (short *)malloc(sig*sizeof(short));
	float ES[2];
	float ESUp, ESDown;
	short index[MAX_GENE];
	
	for(i=0; i<sig ; i++){
		gsUp[i] = s1[i];
		gsDown[i] = s1[len-i-1];
	}
	getIndex(s2,index,len);	
	ESUp = ES_GeneSet( gsUp, index , len, sig);
	ESDown = ES_GeneSet( gsDown, index , len, sig);
	ES[0] =  ( ESUp - ESDown )/2;
	
	for(i=0; i<sig ; i++){
		gsUp[i] = s2[i];
		gsDown[i] = s2[len-i-1];
	}
	getIndex(s1,index,len);	
	ESUp = ES_GeneSet( gsUp, index , len, sig);
	ESDown = ES_GeneSet( gsDown, index , len, sig);
	ES[1] =  ( ESUp - ESDown )/2;	
		
	free(gsUp);
	free(gsDown);	
	return (ES[0]+ES[1])/2;
}

//get the struct Profile_triple from gene profile
struct Profile_triple getTriple(short s[], int len, int sig){
	int i;
	struct Profile_triple triple;
	for(i=0; i<sig ; i++){
		triple.gsUp[i] = s[i];
		triple.gsDown[i] = s[len-i-1];
	}
	getIndex(s,triple.index,len);
	return triple;
}

//compute the ES of profile-pair by struct Profile_triple
float ES_Profile_triple(struct Profile_triple s1, struct Profile_triple s2 ,int len,int sig)
{

	float ES[2];
	float ESUp, ESDown;
	int i;
	
	ESUp = ES_GeneSet( s1.gsUp, s2.index , len, sig);
	ESDown = ES_GeneSet( s1.gsDown, s2.index , len, sig);
	ES[0] =  ( ESUp - ESDown )/2;
	
	ESUp = ES_GeneSet( s2.gsUp, s1.index , len, sig);
	ESDown = ES_GeneSet( s2.gsDown, s1.index , len, sig);
	ES[1] =  ( ESUp - ESDown )/2;		
		
	return (ES[0]+ES[1])/2;
}


