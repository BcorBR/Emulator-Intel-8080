#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(){
    uint16_t a = 0b00000001;
    uint16_t b = 0b00000001;
    
    // testes, ~x turns bits correctly


    // testing negative result
    uint16_t c = a - b;
    uint16_t d = a + (~b + 1);

    printf("%016b", d);

    
    
    return 0;
}
