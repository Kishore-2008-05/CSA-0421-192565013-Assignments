#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define FILES 1000
#define BLOCKS_PER_FILE 256
int main(void){
    /* Compare two read strategies for random time seeks in fixed-size video segments.
       A direct index uses one lookup; a linked chain traverses block number k. */
    uint32_t x=192565013u; unsigned long long direct_reads=0,linked_reads=0;
    for(int i=0;i<100000;i++){x=x*1664525u+1013904223u;unsigned k=x%BLOCKS_PER_FILE;direct_reads+=1;linked_reads+=(unsigned long long)k+1;}
    printf("Queries: 100000; blocks/file: %d\n",BLOCKS_PER_FILE);
    printf("Indexed/extent data-block reads: %llu\n",direct_reads);
    printf("Linked allocation data-block reads: %llu\n",linked_reads);
    printf("Average linked reads/query: %.2f\n",(double)linked_reads/100000.0);
    printf("Interpretation: direct index lookup has constant block-index depth; linked traversal grows with k.\n");
    return 0;
}
