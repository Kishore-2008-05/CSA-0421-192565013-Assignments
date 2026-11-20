#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define LEN 100000
#define MAXF 10
static unsigned rng=192565013u;
static unsigned rnd(void){rng=rng*1664525u+1013904223u;return rng;}
static int find(const int*f,int n,int p){for(int i=0;i<n;i++)if(f[i]==p)return i;return -1;}
static int fifo(const int*r,int n,int cap){int f[MAXF],used=0,next=0,miss=0;for(int i=0;i<n;i++){if(find(f,used,r[i])>=0)continue;miss++;if(used<cap)f[used++]=r[i];else{f[next]=r[i];next=(next+1)%cap;}}return miss;}
static int lru(const int*r,int n,int cap){int f[MAXF],last[MAXF],used=0,miss=0;for(int i=0;i<n;i++){int k=find(f,used,r[i]);if(k>=0){last[k]=i;continue;}miss++;if(used<cap){k=used;f[used]=r[i];last[used]=i;used++;}else{int v=0;for(int j=1;j<cap;j++)if(last[j]<last[v])v=j;f[v]=r[i];last[v]=i;}}return miss;}
static int opt(const int*r,int n,int cap){int f[MAXF],used=0,miss=0;for(int i=0;i<n;i++){if(find(f,used,r[i])>=0)continue;miss++;if(used<cap){f[used++]=r[i];continue;}int v=0,far=-1;for(int j=0;j<cap;j++){int k=i+1;while(k<n&&r[k]!=f[j])k++;if(k>far){far=k;v=j;}}f[v]=r[i];}return miss;}
int main(void){
 static int local[LEN],randoms[LEN];
 rng=192565013u;
 for(int i=0;i<LEN;i++){unsigned x=rnd();local[i]=(x%100<80)?(int)((x>>8)%8):(int)((x>>16)%100);randoms[i]=(int)(rnd()%100);}
 puts("workload,frames,FIFO_faults,LRU_faults,Optimal_faults");
 for(int f=3;f<=10;f++){
   printf("locality,%d,%d,%d,%d\n",f,fifo(local,LEN,f),lru(local,LEN,f),opt(local,LEN,f));
   printf("random,%d,%d,%d,%d\n",f,fifo(randoms,LEN,f),lru(randoms,LEN,f),opt(randoms,LEN,f));
 }
 clock_t a=clock();volatile int sink=fifo(local,LEN,8)+lru(local,LEN,8)+opt(local,LEN,8);clock_t b=clock();
 fprintf(stderr,"100000-reference three-policy runtime: %.3f ms (sink=%d)\n",1000.0*(double)(b-a)/CLOCKS_PER_SEC,sink);
 return 0;
}
