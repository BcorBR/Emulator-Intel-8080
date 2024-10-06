#include <time.h>
#include <stdio.h>

int main() {
    
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    printf("\n%ld\n", start.tv_nsec);

    for (unsigned long int i = 0; i < 4294967295; i ++){
        i = i;
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("%ld\n", start.tv_nsec);


    return 0;
}