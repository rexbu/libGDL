#include "bs_conf.h"

int bs_conf_read(bs_conf_t* pconf, const char* path){
    FILE*               file;
    char                buf[BS_LONG_STRLEN] = {0};
    _conf_elem_t        elem;
    int                 res;

    vector_init(pconf);
    file=fopen(path, "r");
    if(file == NULL){
        return BS_NOTFOUND;
    }

    while(fgets(buf, BS_LONG_STRLEN, file)){
        if(buf[0] == '#'){
            continue;
        }
        char* sig = strchr(buf, '=');
        if(sig==NULL){
            sig = strchr(buf, ':');
        }
        if(sig==NULL){
            continue;
        }
        char* name_sig = sig-1;
        while(name_sig-buf>=0 && bs_is_space(*name_sig)){
            name_sig--;
        }
        
        memset(&elem, 0, sizeof(_conf_elem_t));
        memcpy(elem.name, buf, name_sig-buf+1);
        elem.name[_CONF_NAME_SIZE-1]='\0';
        char* value_sig = sig+1;
        while(value_sig<buf+BS_LONG_STRLEN && bs_is_space(*value_sig)){
            value_sig++;
        }
        /* 越界 bushaofeng 2013.6.17
        memcpy(elem.value, value_sig, buf+BS_LONG_STRLEN-value_sig);
        elem.value[BS_LONG_STRLEN-1]='\0';
        */
        int value_len = strlen(value_sig)>_CONF_VALUE_SIZE? _CONF_VALUE_SIZE:strlen(value_sig);
        memcpy(elem.value, value_sig, value_len);
        elem.value[_CONF_VALUE_SIZE-1]='\0';

        int len = strlen(elem.value);
        if(!bs_is_char(elem.value[len-1])){
            elem.value[len-1]='\0';
        }
        if((res=vector_push(pconf, elem)) != BS_SUCCESS){
            fclose(file);
            return res;
        }
    }
    fclose(file);

    return BS_SUCCESS;
}

const char* bs_conf_getstr(const bs_conf_t* pconf, const char* name){
    uint32_t            i;
    _conf_elem_t*       elem;

//    assert(pconf!=NULL);
//    for(i=0; i<vector_count(pconf); i++){
//        elem = (_conf_elem_t*)vector_index(pconf, i);
//        if(strcmp(elem->name, name) == 0){
//            return elem->value;
//        }
//    }

    return NULL;
}

int bs_conf_getint(const bs_conf_t* pconf, const char* name){
    uint32_t            i;
    _conf_elem_t*       elem;

    for(i=0; i<vector_count(pconf); i++){
//        elem = (_conf_elem_t*)vector_index(pconf, i);
//        if(strcmp(elem->name, name) == 0){
//            return *(int*)elem->value;
//        }
    }

    return -1;
}

size_t bs_conf_size(const bs_conf_t* pconf){
    // return vector_vlen(pconf);
    return 0;
}

const char* bs_conf_getname(const bs_conf_t* pconf, uint32_t i){
//    if(i>=vector_vlen(pconf)){
//        return NULL;
//    }
//
//    _conf_elem_t* elem = (_conf_elem_t*)vector_index(pconf, i);
//    return elem->name;
    return NULL;
}

const char* bs_conf_getstr_idx(const bs_conf_t* pconf, uint32_t i){
//    if(i>=vector_vlen(pconf)){
//        return NULL;
//    }
//
//    _conf_elem_t* elem = (_conf_elem_t*)vector_index(pconf, i);
//    return elem->value;
    return NULL;
}

int bs_conf_getint_idx(const bs_conf_t* pconf, uint32_t i){
//    if(i>=vector_vlen(pconf)){
//        return -1;
//    }
//
//    _conf_elem_t* elem = (_conf_elem_t*)vector_index(pconf, i);
//    return atoi(elem->value);
    return 0;
}

state_t bs_conf_set(bs_conf_t* pconf, const char* name, const void* value, uint32_t size){
//    int                 i;
//    _conf_elem_t*       elem;
//    _conf_elem_t        new_elem;
//    
//    for (i=0; i< vector_vlen(pconf); i++) {
//        elem = (_conf_elem_t*)vector_index(pconf, i);
//        if(strcmp(elem->name, name) == 0){
//            memset(elem->value, 0, _CONF_VALUE_SIZE);
//            bs_memcpy(elem->value, _CONF_VALUE_SIZE, value, size);
//            return BS_SUCCESS;
//        }
//    }
//    
//    memset(&new_elem, 0, sizeof(_conf_elem_t));
//    bs_strcpy(new_elem.name, _CONF_NAME_SIZE, name);
//    bs_memcpy(new_elem.value, _CONF_VALUE_SIZE, value, size);
//    vector_push(pconf, new_elem);
    
    return BS_SUCCESS;
}

state_t bs_conf_setstr(bs_conf_t* pconf, const char* name, const char* value){
    return bs_conf_set(pconf, name, value, strlen(value));
}

state_t bs_conf_setint(bs_conf_t* pconf, const char* name, int value){
    return bs_conf_set(pconf, name, &value, sizeof(value));
}
