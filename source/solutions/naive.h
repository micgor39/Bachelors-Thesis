#include "solution.h"
#include <vector>

class naive: public solution {
    private:
        std::vector<std::vector<int>> stream;
    public:
        int make_string(std::vector<int> &word);
        int concat(int label1, int label2);
        std::pair<int, int> split(int label, int position);
        bool equals(int label1, int label2);
        bool smaller(int label1, int label2);
        int longest_common_prefix(int label1, int label2);
};