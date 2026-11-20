#include <stdio.h>
#include <stdlib.h>
#define MEM 10000
#define SEG 20
int main(void){
    int mem[MEM]={0},base[SEG],size[SEG];srand(192565013u);
    for(int s=0;s<SEG;s++){
        size[s]=100+rand()%501;base[s]=-1;
        for(int b=0;b+size[s]<=MEM;b++){int ok=1;for(int k=b;k<b+size[s];k++)if(mem[k]){ok=0;b=k;break;}if(ok){base[s]=b;for(int k=b;k<b+size[s];k++)mem[k]=s+1;break;}}
        if(base[s]<0){printf("Segment %d (%d units): allocation failed\n",s,size[s]);continue;}
        printf("Segment %d: base=%d size=%d\n",s,base[s],size[s]);
    }
    for(int s=0;s<SEG;s+=3)if(base[s]>=0)for(int k=base[s];k<base[s]+size[s];k++)mem[k]=0;
    int free_total=0,largest=0,run=0;
    for(int i=0;i<MEM;i++){if(!mem[i]){free_total++;run++;if(run>largest)largest=run;}else run=0;}
    printf("After removing every third segment: free=%d units, largest hole=%d units\n",free_total,largest);
    if(free_total)printf("External-fragmentation indicator (1-largest/free): %.4f\n",1.0-(double)largest/free_total);
    return 0;
}
