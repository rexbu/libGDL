/**
 * file :	GDLStyle.cpp
 * author :	Rex
 * create :	2018-01-28 00:08
 * func : 
 * history:
 */

#include "bs.h"
#include "GDLGlobal.h"
#include "GDLStyle.h"
#include "GPUColorSpread.h"
#include "GPUDeColor.h"

GDLStyle::GDLStyle(const char* model):
GDLSession(model){
}

void GDLStyle::encoder(){
    GPUColorSpread* color = new GPUColorSpread(3);
    this->addLayerBase(color);
    GPUPadding* padding = new GPUPadding(3, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding, "pre_padding");
    
    GPUConvLayer* conv = new GPUConvLayer(3, 64);
    this->addConv(conv, "conv1_1");
    padding = new GPUPadding(64, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 64);
    this->addConv(conv, "conv1_2");
    GPUPoolLayer* pool = new GPUPoolLayer(64);
    this->addPool(pool, "pool1");

    padding = new GPUPadding(64, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 128);
    this->addConv(conv, "conv2_1");
    padding = new GPUPadding(128, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 128);
    this->addConv(conv, "conv2_2");
    pool = new GPUPoolLayer(128);
    this->addPool(pool, "pool2");

    padding = new GPUPadding(128, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 256);
    this->addConv(conv, "conv3_1");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 256);
    this->addConv(conv, "conv3_2");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 256);
    this->addConv(conv, "conv3_3");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    conv = new GPUConvLayer(3, 256);
    this->addConv(conv, "conv3_4");
    pool = new GPUPoolLayer(256);
    this->addPool(pool, "pool3");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    m_encoder_4_1 = new GPUConvLayer(3, 512);
    this->addConv(m_encoder_4_1, "conv4_1");

    m_mean = new GPUMean();
    this->addLayerBase(m_mean, "mean");
    m_var = new GPUVariance();
    this->addLayerBase(m_var, "var");
    m_encoder_4_1->addTarget(m_var, 1);
}

void GDLStyle::adain(){
    m_ada = new GPUAdaIn();
    this->addLayerBase(m_ada, "adain");
    m_mean->addTarget(m_ada,1);
    m_encoder_4_1->addTarget(m_ada, 2);
}

void GDLStyle::decoder(){
    GPUPadding* padding = new GPUPadding(512, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding, "decode_padding");
    GPUConvLayer* dconv = new GPUConvLayer(3, 256);
    this->addConv(dconv, "dconv4_1");
    GPUUnSample* unsample = new GPUUnSample();
    this->addLayerBase(unsample, "unsample_0");
    
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 256);
    this->addConv(dconv, "dconv3_4");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 256);
    this->addConv(dconv, "dconv3_3");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 256);
    this->addConv(dconv, "dconv3_2");
    padding = new GPUPadding(256, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 128);
    this->addConv(dconv, "dconv3_1");
    unsample = new GPUUnSample();
    this->addLayerBase(unsample, "unsample_4");
    
    padding = new GPUPadding(128, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 128);
    this->addConv(dconv, "dconv2_2");
    padding = new GPUPadding(128, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 64);
    this->addConv(dconv, "dconv2_1");
    unsample = new GPUUnSample();
    this->addLayerBase(unsample, "unsample_6");
    
    padding = new GPUPadding(64, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 64);
    this->addConv(dconv, "dconv1_2");
    padding = new GPUPadding(64, 1, CONV_PADDING_REFLECT);
    this->addLayerBase(padding);
    dconv = new GPUConvLayer(3, 3, CONV_ACTIVE_NULL);
    this->addConv(dconv, "dconv1_1");
    
    GPUDeColor* decolor = new GPUDeColor();
    this->addLayerBase(decolor);
}

void GDLStyle::style(const char* style_path){
    struct stat statbuff;
    
    if(stat(style_path, &statbuff) < 0){
        err_log("Read Style Model Error:%s", style_path);
        return;
    }
    
    float* buffer = (float*)malloc(statbuff.st_size);
    FILE* fp = fopen(style_path, "r");
    fread(buffer, statbuff.st_size, 1, fp);
    fclose(fp);
    style(buffer);
    
    free(buffer);
}

void GDLStyle::style(float* style){
    encoder();
    adain();
    decoder();
    float* mean = style;
    // 此时mean的outbuffer还是0
    float* var = style+(m_encoder_4_1->m_outx_count*m_encoder_4_1->m_outy_count)*4;
    m_ada->setStyle(mean, var, m_encoder_4_1->m_outx_count, m_encoder_4_1->m_outy_count);
}

void GDLStyle::saveStyle(const char* path){
    size_t size = (size_t)m_mean->m_outbuffer->m_width*(size_t)m_mean->m_outbuffer->m_height*4*sizeof(float);
    float* mean = (float*)malloc(size);
    m_mean->m_outbuffer->getPixels((uint8_t*)mean);
    printf("mean: %f,%f,%f,%f\n", mean[0], mean[1], mean[2], mean[3]);
    FILE* fp = fopen(path, "wb");
    fwrite(mean, size, 1, fp);
    m_var->m_outbuffer->getPixels((uint8_t*)mean);
    printf("var: %f,%f,%f,%f\n", mean[0], mean[1], mean[2], mean[3]);
    fwrite(mean, size, 1, fp);
    fclose(fp);
}

GDLStyle::~GDLStyle(){
    for (int i=0; i<this->layerCount(); i++) {
        delete this->layer(i);
    }
}

void GDLStyle::loadStyleImage(const char* path){
    
}
void GDLStyle::loadStyleModel(const char* path){
    
}
