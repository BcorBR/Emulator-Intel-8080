#include "file.h"

buffer *file(int argc, char *argv[]){
    if (argc < 2){
        printf("ROM file is missing\n");
        exit(1);
    }

    // points to ROM file to be read
    FILE *f;
    f = fopen(argv[1], "rb");
    if (f == NULL){
        printf("Error opening file");
        exit(1);
    }    

    // find file size and put it into buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    // allocating memory for information to be stored from file
    buffer *buf = (buffer *)malloc(sizeof(buffer));
    buf->data = (unsigned char *)malloc(fsize);
    if (buf == NULL){
        printf("Error alocating buffer");
        fclose(f);
        exit(1);
    }
    if (buf->data == NULL){
        printf("Error allocating data on buffer\n");
        fclose(f);
        exit(1);
    }

    fread(buf->data, fsize, 1, f);
    buf->size = fsize;

    fclose(f);

    return buf;
}