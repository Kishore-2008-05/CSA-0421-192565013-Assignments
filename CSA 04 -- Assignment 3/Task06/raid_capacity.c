#include <stdio.h>
int main(void) {
    const double n=6, disk=4.0;
    printf("Six 4-TB disks (nominal decimal capacity):\n");
    printf("RAID 0: %.1f TB usable; no redundancy\n",n*disk);
    printf("RAID 1 (three mirrored pairs): %.1f TB usable; survives one disk per mirror pair\n",n*disk/2);
    printf("RAID 5: %.1f TB usable; survives any one disk failure\n",(n-1)*disk);
    printf("RAID 6: %.1f TB usable; survives any two disk failures\n",(n-2)*disk);
    printf("RAID 10: %.1f TB usable; survives at least one failure, depending on mirror placement\n",n*disk/2);
    return 0;
}
