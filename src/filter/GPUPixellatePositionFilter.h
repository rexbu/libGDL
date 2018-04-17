//
//  GPUPixellatePositionFilter.hpp
//  libGPU
//
//  Created by zhoubl on 2017/9/25.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef GPUPixellatePositionFilter_hpp
#define GPUPixellatePositionFilter_hpp

#include "GPUFilter.h"

class GPUPixellatePositionFilter : public GPUFilter {
public:
  GPUPixellatePositionFilter(float blockSize = 0.05, float ratio = 1.33333);
  
  virtual void setExtraParameter(float blockSize);
  void adjustPixellate(gpu_point_t center, float radius);
};
#endif /* GPUPixellatePositionFilter_hpp */
