/**
 * file :	ConvFrameBuffer.cpp
 * author :	Rex
 * create :	2017-12-20 19:46
 * func : 
 * history:
 */

#include "ConvFrameBuffer.h"

float* ConvFrameBuffer::poolVertices(){
    m_vertices.clear();
    m_indexs.clear();
    
    float w_channel_step = 2.0*m_channel_width/m_width;
    float w_padding_step = 2.0*m_padding_size/m_width;
    float h_channel_step = 2.0*m_channel_height/m_height;
    float h_padding_step = 2.0*m_padding_size/m_height;
    
    // 使用GL_TRIANGLES绘制，每个矩形由2个三角形共6个顶点组成
    for (int h=0; h<m_y_count; h++) {
        float lt_y = -1.0 + (h_channel_step + 2*h_padding_step)*h + h_padding_step;
        for (int w=0; w<m_x_count && h*m_x_count+w<m_channel_count; w++) {
            float lt_x = -1.0 + (w_channel_step + 2*w_padding_step)*w + w_padding_step;
            float rt_x = lt_x + w_channel_step;
            float rt_y = lt_y;
            float lb_x = lt_x;
            float lb_y = lt_y + h_channel_step;
            float rb_x = rt_x;
            float rb_y = lb_y;
            float triangle[] = {lt_x, lt_y, rt_x, rt_y, lb_x, lb_y, rb_x, rb_y};
            for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
                m_vertices.push_back(triangle[i]);
            }
            short index[] = {0,1,2,1,2,3};
            short starti = (h*m_x_count + w)*4;
            for (int i=0; i<6; i++) {
                m_indexs.push_back(starti+index[i]);
            }
        }
    }
    
    return &m_vertices[0];
}

float* ConvFrameBuffer::poolCoordinates(){
    m_coordinates.clear();
    
    float w_channel_step = 1.0*m_channel_width/m_width;
    float w_padding_step = 1.0*m_padding_size/m_width;
    float h_channel_step = 1.0*m_channel_height/m_height;
    float h_padding_step = 1.0*m_padding_size/m_height;
    
    // 使用GL_TRIANGLES绘制，每个矩形由2个三角形共6个顶点组成
    for (int h=0; h<m_y_count; h++) {
        for (int w=0; w<m_x_count && h*m_x_count+w<m_channel_count; w++) {
            float lb_x = (w_channel_step + 2*w_padding_step)*w + w_padding_step;
            float lb_y = (h_channel_step + 2*h_padding_step)*h + h_padding_step;
            float rb_x = lb_x + w_channel_step;
            float rb_y = lb_y;
            float lt_x = lb_x;
            float lt_y = lb_y + h_channel_step;
            float rt_x = rb_x;
            float rt_y = lt_y;
            float triangle[] = {lb_x, lb_y, rb_x, rb_y, lt_x, lt_y, rt_x, rt_y};
            // 第一个三角形坐标
            for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
                m_coordinates.push_back(triangle[i]);
            }
        }
    }
    
    return &m_coordinates[0];
}

void ConvFrameBuffer::triangles_copy(vector<float>& v, float* p){
    int index[6] = {0,1,2,1,2,3};
    for (int i=0; i<sizeof(index)/sizeof(int); i++) {
        v.push_back(p[index[i]]);
    }
}
float* ConvFrameBuffer::reflectVertices(){
    m_vertices.clear();
    m_indexs.clear();
    
    float w_channel_step = 2.0*m_channel_width/m_width;
    float w_padding_step = 2.0*m_padding_size/m_width;
    float h_channel_step = 2.0*m_channel_height/m_height;
    float h_padding_step = 2.0*m_padding_size/m_height;
    /*  0--1----2--3
     *  |  |    |  |
     *  4--5----6--7
     *  |  |    |  |
     *  8--9---10--11
     *  |  |    |  |
     *  12-13--14--15
     */
    for (int h=0; h<m_y_count; h++) {
        for (int w=0; w<m_x_count; w++) {
            float start_x = -1.0 + w*(w_channel_step+2*w_padding_step);
            float start_y = -1.0 + h*(h_channel_step+2*h_padding_step);
            
            float p[16][2];
            // 只计算所有点的x
            for (int ph=0; ph<4; ph++) {
                p[ph*4][0] = start_x;
                p[ph*4+1][0] = start_x + w_padding_step;
                p[ph*4+2][0] = p[ph*4+1][0] + w_channel_step;
                p[ph*4+3][0] = p[ph*4+2][0] + w_padding_step;
            }
            // 只计算所有点的y
            for (int pw=0; pw<4; pw++) {
                p[pw][1] = start_y;
                p[pw+4][1] = start_y+h_padding_step;
                p[pw+8][1] = p[pw+4][1]+h_channel_step;
                p[pw+12][1] = p[pw+8][1]+h_padding_step;
            }

            // 镜像有中转轴，每个矩形的顶点不共用
            uint16_t rect[][4] = {{0,1,4,5},{1,2,5,6},{2,3,6,7},{4,5,8,9},{5,6,9,10},{6,7,10,11},{8,9,12,13},{9,10,13,14},{10,11,14,15}};
            int index[6] = {0,1,2,1,2,3};
            for (int r=0; r<9; r++) {
                for (int pi=0; pi<4; pi++) {
                    m_vertices.push_back(p[rect[r][pi]][0]);
                    m_vertices.push_back(p[rect[r][pi]][1]);
                }
                uint16_t start_i = (h*m_x_count+w)*36+r*4;
                for (int ii=0; ii<6; ii++) {
                    m_indexs.push_back(start_i+index[ii]);
                }
            }
        }
    }
    
    return &m_vertices[0];
}

float* ConvFrameBuffer::reflectCoordinates(int padding){
    m_coordinates.clear();
    
    float w_channel_step = 1.0*m_channel_width/m_width;
    float w_padding_step = 1.0*m_padding_size/m_width;
    float h_channel_step = 1.0*m_channel_height/m_height;
    float h_padding_step = 1.0*m_padding_size/m_height;
    
    float w_nextpad_step = 1.0*padding/m_width;
    float h_nextpad_step = 1.0*padding/m_height;
    /*  0--1----2--3
     *  |  |    |  |
     *  4--5----6--7
     *  |  |    |  |
     *  8--9---10--11
     *  |  |    |  |
     *  12-13--14--15
     *  对应纹理坐标如下，右侧和下侧是经过卷积计算后需要废弃的padding部分
     *  5-|-|--|-|-6**
     *  |-5-4--7-6-|
     *  | | |  | | |**
     *  |-1-0  3-2-|**
     *  |   |  | | |**
     *  |-13121514-|**
     *  | | |  | | |**
     *  |-9-8-11-10-|
     *  9--8--11-10**
     *  *************
     *  *************
     
     * 0 -1 -2--3 -4 -5
     * 6 -7 -8--9 -10-11
     * |  |  |  |  |  |
     * 12-13-14 15-16-17
     * |              |
     * 18-19-20 21-22-23
     * |  |  |  |  |  |
     * 24-25-26 27-28-29
     * 30-31-32-33-34-35
     */
    for (int h=0; h<m_y_count; h++) {
        for (int w=0; w<m_x_count; w++) {
            float start_x = w*(w_channel_step+2*w_padding_step);
            float start_y = h*(h_channel_step+2*h_padding_step);
            // 左上
            
            float p[36][2];
            int rect[][4] = {{14,13,8,7},{12,17,6,11},{16,15,10,9},{2,1,32,31},{0,5,30,35},{4,3,34,33},{26,25,20,19},{24,29,18,23},{28,27,22,21}};
            for (int hi=0; hi<6; hi++) {
                p[hi*6][0] = start_x;
                p[hi*6+1][0] = start_x + 1.0/m_width;
                p[hi*6+2][0] = p[hi*6+1][0] + w_nextpad_step;
                p[hi*6+5][0] = start_x+w_channel_step;
                p[hi*6+4][0] = p[hi*6+5][0] - 1.0/m_width;
                p[hi*6+3][0] = p[hi*6+4][0] - w_nextpad_step;
            }
            for (int wi=0; wi<6; wi++) {
                p[wi][1] = start_y;
                p[6+wi][1] = start_y + 1.0/m_height;
                p[12+wi][1] = p[6+wi][1] + h_nextpad_step;
                p[30+wi][1] = start_y + h_channel_step;
                p[24+wi][1] = p[30+wi][1] - 1.0/m_height;
                p[18+wi][1] = p[24+wi][1] - h_nextpad_step;
            }
            
            for (int ri=0; ri<9; ri++) {
                for (int ii=0; ii<4; ii++) {
                    m_coordinates.push_back(p[rect[ri][ii]][0]);
                    m_coordinates.push_back(p[rect[ri][ii]][1]);
                }
            }
        }
    }
    
    return &m_coordinates[0];
}

float* ConvFrameBuffer::convVertices(){
    m_vertices.clear();
    m_indexs.clear();
    
    float w_channel_step = 2.0*m_channel_width/m_width;
    float w_padding_step = 2.0*m_padding_size/m_width;
    float h_channel_step = 2.0*m_channel_height/m_height;
    float h_padding_step = 2.0*m_padding_size/m_height;
    
    // 使用GL_TRIANGLES绘制，每个矩形由2个三角形共6个顶点组成
    for (int h=0; h<m_y_count; h++) {
        for (int w=0; w<m_x_count && h*m_x_count+w<m_channel_count; w++) {
            float p[4][2];
            p[0][0] = -1.0 + (w_channel_step + 2*w_padding_step)*w;
            p[0][1] = -1.0 + (h_channel_step + 2*h_padding_step)*h;
            p[1][0] = p[0][0] + w_channel_step;
            p[1][1] = p[0][1];
            p[2][0] = p[0][0];
            p[2][1] = p[0][1]  + h_channel_step;
            p[3][0] = p[1][0];
            p[3][1] = p[2][1];
            for (int i=0; i<4; i++) {
                m_vertices.push_back(p[i][0]);
                m_vertices.push_back(p[i][1]);
            }
            
            uint16_t index[] = {0, 1, 2, 1, 2, 3};
            // 计算索引
            uint16_t start_i = (h*m_x_count+w)*4;
            for (int i=0; i<6; i++) {
                m_indexs.push_back(start_i+index[i]);
            }
        }
    }
    
    return &m_vertices[0];
}
float* ConvFrameBuffer::convCoordinates(){
    m_coordinates.clear();
    float w_channel_step = 1.0*m_channel_width/m_width;
    float w_padding_step = 1.0*m_padding_size/m_width;
    float h_channel_step = 1.0*m_channel_height/m_height;
    float h_padding_step = 1.0*m_padding_size/m_height;
    
    for (int h=0; h<m_y_count; h++) {
        float lt_y = (h_channel_step + 2*h_padding_step)*h;
        for (int w=0; w<m_x_count && h*m_x_count+w<m_channel_count; w++) {
            float lt_x = (w_channel_step + 2*w_padding_step)*w;
            float rt_x = lt_x + w_channel_step;
            float rt_y = lt_y;
            float lb_x = lt_x;
            float lb_y = lt_y + h_channel_step;
            float rb_x = rt_x;
            float rb_y = lb_y;
            float triangle[] = {lt_x, lt_y, rt_x, rt_y, lb_x, lb_y, rb_x, rb_y};
            for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
                m_coordinates.push_back(triangle[i]);
            }
        }
    }
    
    return &m_coordinates[0];
}
