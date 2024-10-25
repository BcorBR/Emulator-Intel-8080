#include <stdio.h>
#include <stdlib.h>

typedef struct buffer{
    unsigned char *data;
    size_t size;
} buffer;

buffer *file(int argc, char *argv[]);