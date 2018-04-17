/**
 * file :	GDLModel.cpp
 * author :	Rex
 * create :	2018-01-27 22:38
 * func : 
 * history:
 */

#include "GDLModel.h"

GDLModel::GDLModel(const char* path){
    m_buffer = NULL;
    m_loaded = loadModel(path);
}

GDLModel::~GDLModel(){
    if (m_buffer!=NULL) {
        m_layers.clear();
        free(m_buffer);
    }
}

bool GDLModel::loadModel(const char* path){
    struct stat statbuff;
    
    if(stat(path, &statbuff) >= 0){
        m_buffer = (uint8_t*)malloc(statbuff.st_size);
        FILE* fp = fopen(path, "r");
        fread(m_buffer, statbuff.st_size, 1, fp);
        fclose(fp);
        return loadModel(m_buffer, (uint32_t)statbuff.st_size);
    }
    return false;
}

bool GDLModel::loadModel(uint8_t* bytes, uint32_t length){
    uint8_t* ptr = bytes;
    while (ptr<bytes+length) {
        conv_layer_t layer;
        layer.conv_name = (char*)ptr;
        ptr += strlen(layer.conv_name)+1;
        memcpy(&layer.kernel_width, ptr, sizeof(conv_layer_t)-sizeof(char*));
        layer.weight = (float*)(ptr + sizeof(int)*5);
        layer.bias = layer.weight + layer.kernel_width*layer.kernel_height*layer.channel_count*layer.kernel_count;
        ptr = (uint8_t*)(layer.bias+layer.kernel_count);
        m_layers[string(layer.conv_name)] = layer;
    }
    m_loaded = true;
    return true;
}

conv_layer_t* GDLModel::layer(const char* name){
    map<string, conv_layer_t>::iterator iter = m_layers.find(string(name));
    if (iter != m_layers.end()) {
        return &iter->second;
    }
    return NULL;
}
