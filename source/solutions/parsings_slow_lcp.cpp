#include "parsings_slow_lcp.h"

parsings_slow_lcp::~parsings_slow_lcp() {}

int parsings_slow_lcp::longest_common_prefix(int label1, int label2) {
    int max_possible_lcp = std::min(get_length(label1), get_length(label2));
    // binary search the answer
    int left_pointer = 0, right_pointer = max_possible_lcp + 1;
    while(left_pointer + 1 < right_pointer) {
        int guess = (left_pointer + right_pointer) / 2;
        auto s1 = context_insensitive_decomposition(label1, 1, guess);
        auto s2 = context_insensitive_decomposition(label2, 1, guess);
        if(s1 == s2) {
            left_pointer = guess;
        } else {
            right_pointer = guess;
        }
    }
    return left_pointer;
}