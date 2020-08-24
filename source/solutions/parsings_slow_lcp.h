#include "parsings.h"

class parsings_slow_lcp: public parsings {
    public:
        using parsings::parsings;
        ~parsings_slow_lcp();
        int longest_common_prefix(int label1, int label2);
};