#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/resource.h>
static double now_us(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec*1e6+t.tv_nsec/1e3;}
static void run(const char*tag,char*buf,size_t n){
  double worst=0,sum=0;int N=200000;
  for(int i=0;i<N;i++){double a=now_us();buf[(size_t)(rand())%n]++;double d=now_us()-a;sum+=d;if(d>worst)worst=d;}
  printf("%-9s avg %.3f us, worst %.1f us\n",tag,sum/N,worst);
}
int main(void){
  struct rlimit rl;getrlimit(RLIMIT_MEMLOCK,&rl);
  printf("RLIMIT_MEMLOCK soft=%lu hard=%lu bytes\n",(unsigned long)rl.rlim_cur,(unsigned long)rl.rlim_max);
  size_t n=4<<20;char*buf=malloc(n);memset(buf,0,n);srand(1);
  run("unlocked",buf,n);
  double a=now_us();int rc=mlock(buf,n);double lat=now_us()-a;
  if(rc!=0){perror("mlock (error path tested)");printf("mlock of %zu bytes failed; locked run skipped\n",n);return 0;}
  printf("mlock(4 MB) took %.1f us\n",lat);
  run("locked",buf,n);
  a=now_us();munlock(buf,n);printf("munlock took %.1f us\n",now_us()-a);
  return 0;}
