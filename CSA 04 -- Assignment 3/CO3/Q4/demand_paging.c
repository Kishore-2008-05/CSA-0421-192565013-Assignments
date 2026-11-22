#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static long minflt(void){struct rusage r;getrusage(RUSAGE_SELF,&r);return r.ru_minflt;}
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9;}
static long rss_kb(void){long a,b;FILE*f=fopen("/proc/self/statm","r");if(!f||fscanf(f,"%ld %ld",&a,&b)!=2)b=0;if(f)fclose(f);return b*sysconf(_SC_PAGESIZE)/1024;}
int main(void){
  size_t pg=sysconf(_SC_PAGESIZE),n=64<<20,pages=n/pg;
  printf("page size %zu bytes\n",pg);
  char*m=mmap(NULL,n,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  long f0=minflt();double t0=now();
  for(size_t i=0;i<pages;i++)m[i*pg]=1;
  printf("sequential first touch: %ld faults for %zu pages, %.1f ms\n",minflt()-f0,pages,(now()-t0)*1e3);
  f0=minflt();t0=now();srand(7);
  for(size_t i=0;i<pages;i++)m[((size_t)rand()%pages)*pg]++;
  printf("random re-touch (already resident): %ld faults, %.1f ms\n",minflt()-f0,(now()-t0)*1e3);
  char*r=mmap(NULL,n,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  f0=minflt();t0=now();srand(7);
  for(size_t i=0;i<pages;i++)r[((size_t)rand()%pages)*pg]=1;
  printf("random first touch: %ld faults, %.1f ms\n",minflt()-f0,(now()-t0)*1e3);
  /* copy-on-write */
  printf("COW: parent RSS before fork %ld KB\n",rss_kb());
  fflush(stdout);
  pid_t p=fork();
  if(p==0){
    long b=rss_kb();long f1=minflt();
    for(size_t i=0;i<pages;i++)m[i*pg]=2; /* write -> private copies */
    printf("COW child: RSS before write %ld KB, after write %ld KB, write faults %ld\n",b,rss_kb(),minflt()-f1);
    fflush(stdout);_exit(0);}
  wait(NULL);
  /* EAT table */
  double mem=100,svc=8e6;double ps[]={1e-3,1e-5,1e-6};
  for(int i=0;i<3;i++)printf("EAT fault rate %.0e = %.1f ns\n",ps[i],(1-ps[i])*mem+ps[i]*svc);
  printf("max fault rate for 10%% slowdown: %.3e\n",0.1*mem/(svc-mem));
  return 0;}
