#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc,char **argv) {
    size_t mb=argc>1?(size_t)strtoul(argv[1],NULL,10):256;
    long ps=sysconf(_SC_PAGESIZE); size_t bytes=mb*1024*1024, pages=(bytes+(size_t)ps-1)/(size_t)ps;
    volatile unsigned char *p=calloc(pages,(size_t)ps);
    if(!p){perror("calloc");return 1;}
    struct rusage before,after; getrusage(RUSAGE_SELF,&before);
    for(size_t i=0;i<pages;i++) p[i*(size_t)ps]=1;
    getrusage(RUSAGE_SELF,&after);
    printf("Requested memory: %zu MiB; page size: %ld bytes; pages touched: %zu\n",mb,ps,pages);
    printf("Minor faults delta: %ld; major faults delta: %ld\n",after.ru_minflt-before.ru_minflt,after.ru_majflt-before.ru_majflt);
    pid_t child=fork();
    if(child==0){p[0]=2;_exit(0);} else if(child>0) waitpid(child,NULL,0); else perror("fork");
    free((void *)p); return 0;
}
