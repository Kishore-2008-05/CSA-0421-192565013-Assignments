#include <stdio.h>
int main(void){
    const unsigned long long ptrs=4096/4;
    long double blocks=12.0L+ptrs+ptrs*ptrs+ptrs*ptrs*ptrs;
    long double bytes=blocks*4096.0L;
    unsigned long long total_blocks=4000000000000ULL/4096ULL;
    unsigned long long bitmap_bytes=(total_blocks+7ULL)/8ULL;
    printf("4 GiB file blocks: %llu\n",(unsigned long long)(4ULL*1024*1024*1024/4096));
    printf("Pointers per indirect block: %llu\n",ptrs);
    printf("Maximum inode-addressable bytes: %.0Lf\n",bytes);
    printf("4 TB decimal bitmap: %llu bytes (%.2f MiB)\n",bitmap_bytes,(double)bitmap_bytes/(1024.0*1024.0));
    return 0;
}
