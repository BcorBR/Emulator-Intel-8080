#include <stdint.h>
#include <stdio.h>

int main(){
    uint16_t res = 0b00000001 - 0b11111111;

    printf("%b", res);

    return 0;
}