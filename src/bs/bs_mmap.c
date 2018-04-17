/**
 * bushaofeng. 2011.7.18
 * bs_mmap.c
 * history:
 */

#include "bs_mmap.h"

void* bs_mmap_create(int size){
    int         fd;
    void*       pmmap;

    fd = open("/dev/zero", O_RDWR);
    if(fd < 0){
        return NULL;
    }

    pmmap = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);
    
    if(pmmap == MAP_FAILED){
        return NULL;
    }

    return pmmap;
}

void* bs_mmap_file(char* file, size_t size){
    int         fd;
    void*       pmmap;

    fd = open(file , O_RDWR|O_CREAT);
    if(fd < 0){
        return NULL;
    }

    pmmap = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    if(pmmap == MAP_FAILED){
        return NULL;
    }

    return pmmap;
}
