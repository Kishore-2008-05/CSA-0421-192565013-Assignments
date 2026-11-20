#include <stdio.h>
#include <stdlib.h>
#define N 8
#define MAXC 199
static int req[N]={98,183,37,122,14,124,65,67};
static int cmp(const void*a,const void*b){int x=*(const int*)a,y=*(const int*)b;return (x>y)-(x<y);}
static int total(const char*name,const int*a,int n,int head){int p=head,sum=0;printf("%-7s %d",name,head);for(int i=0;i<n;i++){printf(" -> %d",a[i]);sum+=abs(a[i]-p);p=a[i];}printf(" | movement=%d\n",sum);return sum;}
int main(void){
 int a[N+3],s[N];for(int i=0;i<N;i++)s[i]=req[i];qsort(s,N,sizeof s[0],cmp);
 total("FCFS",req,N,53);
 int done[N]={0},p=53;for(int k=0;k<N;k++){int best=-1,dist=MAXC+1;for(int i=0;i<N;i++)if(!done[i]&&abs(req[i]-p)<dist){best=i;dist=abs(req[i]-p);}done[best]=1;a[k]=req[best];p=req[best];}total("SSTF",a,N,53);
 int n=0;for(int i=N-1;i>=0;i--)if(s[i]<53)a[n++]=s[i];a[n++]=0;for(int i=0;i<N;i++)if(s[i]>53)a[n++]=s[i];total("SCAN",a,n,53);
 n=0;for(int i=N-1;i>=0;i--)if(s[i]<53)a[n++]=s[i];for(int i=0;i<N;i++)if(s[i]>53)a[n++]=s[i];total("LOOK",a,n,53);
 n=0;for(int i=0;i<N;i++)if(s[i]>53)a[n++]=s[i];a[n++]=MAXC;for(int i=0;i<N;i++)if(s[i]<53)a[n++]=s[i];int cs=total("C-SCAN",a,n,53)+14;printf("C-SCAN circular wrap correction (+14) => %d\n",cs);
 n=0;for(int i=0;i<N;i++)if(s[i]>53)a[n++]=s[i];for(int i=0;i<N;i++)if(s[i]<53)a[n++]=s[i];total("C-LOOK",a,n,53);
 return 0;
}
