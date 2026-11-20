#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
static double ms(struct timespec a,struct timespec b){return (b.tv_sec-a.tv_sec)*1000.0+(b.tv_nsec-a.tv_nsec)/1e6;}
int main(int argc,char **argv){
    size_t mb=argc>1?(size_t)strtoul(argv[1],NULL,10):8, n=mb*1024*1024;
    unsigned char *p=malloc(n); if(!p){perror("malloc");return 1;}
    struct timespec a,b; struct rusage r0,r1;
    getrusage(RUSAGE_SELF,&r0); clock_gettime(CLOCK_MONOTONIC,&a);
    for(size_t i=0;i<n;i+=4096)p[i]=1;
    clock_gettime(CLOCK_MONOTONIC,&b); getrusage(RUSAGE_SELF,&r1);
    printf("Without mlock: touch %.1f MiB in %.3f ms; minor faults=%ld\n",mb,ms(a,b),r1.ru_minflt-r0.ru_minflt);
    if(mlock(p,n)!=0){perror("mlock (may require a higher RLIMIT_MEMLOCK)");free(p);return 2;}
    getrusage(RUSAGE_SELF,&r0); clock_gettime(CLOCK_MONOTONIC,&a);
    for(size_t i=0;i<n;i+=4096)p[i]++;
    clock_gettime(CLOCK_MONOTONIC,&b); getrusage(RUSAGE_SELF,&r1);
    printf("With mlock: second touch %.3f ms; minor faults=%ld\n",ms(a,b),r1.ru_minflt-r0.ru_minflt);
    munlock(p,n);free(p);return 0;
}
