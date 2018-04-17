/**
 * file :	GDL.h
 * author :	Rex
 * create :	2017-12-06 12:01
 * func : 
 * history:
 */

#ifndef	__GDL_H_
#define	__GDL_H_

#ifdef __cplusplus
extern "C"{
#endif
    
typedef void* gdl_t;
    
extern int gdl_init(const char* license);
extern gdl_t gdl_load(const char* model);
extern float* gdl_process(gdl_t gdl, int* count);
extern void gdl_release(gdl_t gdl);
    
#ifdef __cplusplus
}
#endif
        
#endif
