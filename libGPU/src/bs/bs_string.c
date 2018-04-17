/**
 * file :	bs_string.c
 * author :	bushaofeng
 * create :	2016-08-25 01:03
 * func : 
 * history:
 */

#include "bs_string.h"

void* string_init(void* p){
	string_t* s = (string_t*)p;
	s->mem = (char*)malloc(STRING_DEF_SIZE);
	s->size = STRING_DEF_SIZE;
	s->len = 0;
	s->set = string_set;
	s->append = string_append;
	return s;
}

void string_destroy(void* p){
	string_t* s = (string_t*)p;
	if (s->mem != NULL)
	{
		free(s->mem);
	}
	s->mem = NULL;
	s->size = 0;
}

status_t string_set(struct string_t* str, const char* s){
    if (s==NULL) {
        return 0;
    }
    
	size_t len = strlen(s);
	if (str->size <= len) {
	 	size_t relen = len+1; // 如果len=1，需要额外加1个字节，所有后面+1
	 	// realloc: 如果有足够连续空间，直接在当前指针上扩展空间，如果不足则重新申请空间，同时会释放原空间
	 	char* pnew = (char*)realloc(str->mem, relen);
	 	if (pnew==NULL)
	 	{
	     	return BS_NOMEM;
	 	}

	 	str->mem = pnew;
	 	str->size = (uint32_t)relen;
	}
	strcpy(str->mem, s);

	return (int)len;
}

status_t string_append(string_t* str, const char* s){
	size_t len = strlen(s);
	if (str->size <= str->len+len)
	{
     	size_t relen = len*3/2+1; // 如果len=1，需要额外加1个字节，所有后面+1
     	// realloc: 如果有足够连续空间，直接在当前指针上扩展空间，如果不足则重新申请空间，同时会释放原空间
		char* pnew = (char*)realloc(str->mem, str->size + relen);
		if (pnew==NULL)
		{
			return BS_NOMEM;
		}
	
		str->mem = pnew;
		str->size += relen;
	}
	strcpy(str->mem+str->len, s);
	str->len += len;

	return str->len;
}
