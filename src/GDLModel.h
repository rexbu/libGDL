/**
 * file :	GDLModel.h
 * author :	Rex
 * create :	2018-01-27 22:38
 * func : 
 * history:
 */

#ifndef	__GDLMODEL_H_
#define	__GDLMODEL_H_

#include "bs.h"
#include <iostream>
#include <map>

using namespace std;
// 卷积层
typedef struct conv_layer_t{
    char*       conv_name;
    int         kernel_width;   // 卷积核宽
    int         kernel_height;  // 卷积核高
    union{
        int         channel_count;  // 通道数
        int         active;     // 激励函数类型
    };
    int         kernel_count;   // 卷积核数量
    int         layer_type;     // 0:普通卷积层，1：全连接层
    float*      weight;         // 卷积核矩阵
    float*      bias;           // 偏移向量
}conv_layer_t;

class GDLModel{
public:
    GDLModel(const char* path);
    ~GDLModel();
    
    bool loadModel(const char* path);
    bool loadModel(uint8_t* bytes, uint32_t length);
    inline bool loaded(){ return m_loaded; }
    
    conv_layer_t* layer(const char* name);
protected:
    bool                        m_loaded;
    uint8_t*                    m_buffer;
    map<string, conv_layer_t>   m_layers;
};

#endif
