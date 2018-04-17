//
//  GPUPixellateFilter.hpp
//  libGPU
//
//  Created by zhoubl on 2017/9/26.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef GPUPixellateFilter_hpp
#define GPUPixellateFilter_hpp

#include "GPUFilter.h"

class GPUPixellateFilter : public GPUFilter {
public:
  GPUPixellateFilter(float blockSize = 0.05, float ratio = 1.33333);
  virtual void setExtraParameter(float blockSize);
};

#endif /* GPUPixellateFilter_hpp */
