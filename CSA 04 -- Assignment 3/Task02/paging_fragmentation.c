#include <stdio.h>
#include <stdlib.h>
#define PROCESSES 100
int main(void){
    const unsigned page[]={1024,4096,2u*1024u*1024u};
    unsigned size[PROCESSES];unsigned long long waste[3]={0};
    srand(192565013u);
    for(int i=0;i<PROCESSES;i++)size[i]=1u+(unsigned)rand()%(50u*1024u*1024u);
    for(int i=0;i<PROCESSES;i++)for(int j=0;j<3;j++)waste[j]+=(page[j]-(size[i]%page[j]))%page[j];
    puts("page_size_bytes,total_internal_waste_bytes,average_waste_bytes_per_process");
    for(int j=0;j<3;j++)printf("%u,%llu,%.2f\n",page[j],waste[j],(double)waste[j]/PROCESSES);
    puts("Fixed seed 192565013; process sizes uniformly sampled from 1 byte to 50 MiB.");
    return 0;
}
