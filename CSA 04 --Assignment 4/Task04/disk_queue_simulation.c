#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define N 10000
#define MAXC 199
typedef struct { int cylinder; double arrival; int pending; } Request;
static uint32_t rng=192565013u;
static uint32_t next_u32(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static int run(const char *name,const Request src[N],int sstf){
    Request q[N];for(int i=0;i<N;i++)q[i]=src[i];
    int head=53,done=0;double now=0,sum_wait=0,max_wait=0;long long movement=0;
    while(done<N){
        int pick=-1;
        for(int i=0;i<N;i++)if(q[i].pending && q[i].arrival<=now){
            if(pick<0)pick=i;
            else if(sstf){
                int d=abs(q[i].cylinder-head),best=abs(q[pick].cylinder-head);
                if(d<best || (d==best && q[i].arrival<q[pick].arrival))pick=i;
            }else if(q[i].arrival<q[pick].arrival)pick=i;
        }
        if(pick<0){
            double next=1e100;
            for(int i=0;i<N;i++)if(q[i].pending && q[i].arrival<next)next=q[i].arrival;
            now=next;continue;
        }
        double wait=now-q[pick].arrival;if(wait>max_wait)max_wait=wait;sum_wait+=wait;
        int seek=abs(q[pick].cylinder-head);movement+=seek;
        now+=0.5+(double)seek*0.02;head=q[pick].cylinder;q[pick].pending=0;done++;
    }
    printf("%-5s avg_wait=%.3f ms max_wait=%.3f ms avg_seek=%.3f cylinders/request finish=%.1f ms\n",name,sum_wait/N,max_wait,(double)movement/N,now);
    return 0;
}
int main(void){
    const char *loads[]={"light","medium","heavy"};const double gaps[]={20.0,2.0,0.2};
    for(int l=0;l<3;l++){
        Request q[N];rng=192565013u;
        for(int i=0;i<N;i++){q[i].cylinder=(int)(next_u32()%(MAXC+1));q[i].arrival=(double)i*gaps[l];q[i].pending=1;}
        printf("%s load: %d requests, mean inter-arrival %.1f ms\n",loads[l],N,gaps[l]);
        run("FCFS",q,0);run("SSTF",q,1);
    }
    puts("Model assumptions: seek service=0.02 ms/cylinder plus 0.5 ms transfer; seeded synthetic arrivals, not physical-disk measurements.");
    return 0;
}
