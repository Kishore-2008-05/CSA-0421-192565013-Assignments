#include <stdio.h>
#include <math.h>
int main(void){
    const unsigned long long ptrs=4096/4;
    long double blocks=12.0L+ptrs+ptrs*ptrs+ptrs*ptrs*ptrs;
    long double bytes=blocks*4096.0L;
    printf("4 GiB file blocks: %llu\n",(unsigned long long)(4ULL*1024*1024*1024/4096));
    printf("Pointers per indirect block: %llu\n",ptrs);
    printf("Maximum inode-addressable bytes: %.0Lf\n",bytes);
    printf("4 TB decimal bitmap: %.0Lf bytes (%.2Lf MiB)\n",ceill((4.0e12L/4096.0L)/8.0L),ceill((4.0e12L/4096.0L)/8.0L)/(1024.0L*1024.0L));
    return 0;
}
