#include "bs_def.h"
#include "bs_common.h"

uint64_t atou64(const char* str){
    uint64_t    value = 0;
    uint64_t    factor = 1;
    int         i = 0;
    int         len = (uint32_t)strlen(str);

    if(len == 0){
        return 0;
    }

    for(i=len-1; i>=0; i--){
        if(bs_is_num(str[i])){
            factor = factor*10;
            value += (str[i]-'0')*(factor/10);
        }
        else{
            return -1;
        }
    }

    return value;
}

uint32_t bs_strcpy(void* src, uint32_t src_size, const void* dst){
    uint32_t        size;
    
    if (dst==NULL) {
        memset(src, 0, src_size);
        return 0;
    }
    
    uint32_t        dst_size = (uint32_t)strlen(dst);
    
    memset(src, 0, src_size);
    size = (src_size-1)>dst_size ? dst_size:(src_size-1);
    memcpy(src, dst, size);
    return size;
}

uint32_t bs_memcpy(void* src, uint32_t src_size, const void* dst, uint32_t dst_size){
    uint32_t        size;
    
    size = src_size>dst_size ? dst_size:src_size;
    memcpy(src, dst, size);
    return size;
}

void bs_strswap(void* src, uint32_t src_size, char* dst, uint32_t dst_size){
    char            swap[BS_LONG_STRLEN];
    
    bs_strcpy(swap, BS_LONG_STRLEN, src);
    bs_strcpy(src, src_size, dst);
    bs_strcpy(dst, dst_size, swap);
}

char* bs_strrstr(const char* src, const char* dst){
    char*       ptr = (char*)src;
    char*       rv = NULL;
    size_t      dst_len;
    
    if (src==NULL || dst==NULL) {
        return NULL;
    }
    
    dst_len = strlen(dst);
    while((ptr = strstr(ptr, dst)) != NULL){
        rv = ptr;
        ptr += dst_len;
    }
    
    return rv;
}


int bs_log2(int x){
    float fx;
    unsigned long ix, exp;
    
    fx = (float)x;
    ix = *(unsigned long*)&fx;
    exp = (ix >> 23) & 0xFF;
    
    return exp - 127;
}

#ifdef __LINUX__
state_t convert(const char *fromset, const char *toset, char *from,uint32_t from_len, char *to,uint32_t to_len);
{
    iconv_t cd;
    cd=iconv_open(toset,fromset);
    if(iconv(cd, &from, &from_len, &to, &to_len)==-1){
        iconv_close(cd);
        return BS_INVALID;
    }

    iconv_close(cd);
    return BS_SUCCESS;
}
#endif
