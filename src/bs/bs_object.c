/**
 * file :	bs_object.c
 * author :	bushaofeng
 * create :	2016-08-25 13:22
 * func : 
 * history:
 */

#include "bs_object.h"

void* new_object(const char* name, uint32_t esize, void* (*constructor)(void*), void (*destructor)(void*)){
	object_t* object = (object_t*)malloc(esize);
	if (object==NULL)
	{
		return NULL;
	}

    memset(object, 0, esize);
	object->size = esize;
	memcpy(object->name, name, OBJECT_NAME_SIZE);
	object->constructor = constructor;
	object->destructor = destructor;

	constructor(object);
    object->dynamic = BS_TRUE;
	return object;
}
