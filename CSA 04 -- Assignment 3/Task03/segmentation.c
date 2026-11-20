#include <stdio.h>
typedef struct { const char *name; int base, limit; } Segment;
int main(void) {
    Segment t[]={{"code",1000,400},{"data",5000,800},{"stack",9000,500}};
    struct { int seg, off; } a[]={{0,0},{0,399},{0,400},{1,120},{1,799},{2,499},{2,500},{3,0}};
    for (unsigned i=0;i<sizeof a/sizeof a[0];i++) {
        int s=a[i].seg, off=a[i].off;
        if(s<0 || s>=3) { printf("(%d,%d): invalid segment\n",s,off); continue; }
        if(off<0 || off>=t[s].limit) printf("(%d,%d): protection fault (limit=%d)\n",s,off,t[s].limit);
        else printf("(%d,%d): physical address %d (%s)\n",s,off,t[s].base+off,t[s].name);
    }
    return 0;
}
