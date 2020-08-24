#include "parsings.h"

class parsings_slow_lcp: public parsings {
    public:
        using parsings::parsings;
        int longest_common_prefix(int label1, int label2);
};