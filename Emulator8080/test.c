#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(){
    uint16_t a = 0b00000000;
    uint16_t b = 0b00000001;
    
    // testes, ~x turns bits correctly

    if (a == 0b0)
        printf("YES");
    else
        printf("NO");

    
    
    return 0;
}
