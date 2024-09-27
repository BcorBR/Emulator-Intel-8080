#include <stdlib.h>
#include <stdio.h>

unsigned int concatenar(unsigned int a, unsigned int b){
    
    return (a << 4) | b;
}

int main(){
    unsigned int a = 0b1010;
    unsigned int b = 0b1100;   
    
    unsigned int c = (a << 4) | b;
    
    printf("%b", c);
    
    return 0;
    
    
}
