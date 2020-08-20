#ifndef __SOLUTION_H_INCLUDED__
#define __SOLUTION_H_INCLUDED__
#include <vector>

class solution {
    public:
        virtual int make_string(std::vector<int> &word) = 0;
        virtual int concat(int label1, int label2) = 0;
        virtual std::pair<int, int> split(int label, int position) = 0;
        virtual bool equals(int label1, int label2) = 0;
        virtual bool smaller(int label1, int label2) = 0;
        virtual int longest_common_prefix(int label1, int label2) = 0;
};
#endif