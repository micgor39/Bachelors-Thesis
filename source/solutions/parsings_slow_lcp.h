#ifndef __PARSINGS_SLOW_LCP_H_INCLUDED__
#define __PARSINGS_SLOW_LCP_H_INCLUDED__
#include "parsings.h"

class parsings_slow_lcp : public parsings {
  public:
    using parsings::parsings;
    ~parsings_slow_lcp();
    int longest_common_prefix(int label1, int label2);
};

#endif