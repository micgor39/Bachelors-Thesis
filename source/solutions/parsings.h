#include "solution.h"
#include <vector>
#include <unordered_map>
#include <tuple>
#include <random>
#include <assert.h>

#include <map>
#include <iostream>

class parsings: public solution {
    private:
        bool debug;
        std::mt19937_64 random_number_generator;
        int symbols_counter = 0;
        std::unordered_map<int, std::vector<int>> grammar;
        std::vector<std::pair<int, int>> rules;
        std::vector<int> level;
        std::vector<int> length;
        std::vector<bool> rle;
        std::unordered_map<int, bool> random_bit;
        std::map<std::pair<int, int>, int> inverse_rle_rules;
        std::map<std::pair<int, int>, int> inverse_shrink_rules;
        bool get_random_bit(int symbol);
        int get_length(int symbol);
        int get_level(int symbol);
        bool is_rle(int symbol);
        int create_new_rle_symbol(int symbol, int quantity);
        int create_new_shrink_symbol(int left_symbol, int right_symbol);
        int get_rle_symbol(int symbol, int quantity);
        int get_shrink_symbol(int left_symbol, int right_symbol);
        int get_kth_character(int symbol, int k);
        void get_path_to_kth_leaf(int symbol, int k, std::vector<std::tuple<int, int, int>> &dest, int siblings, bool left, int index);
        std::vector<std::pair<int, int>> context_insensitive_decomposition(int symbol, int from, int to);
        int find_representant(std::vector<std::pair<int, int>> decomposition);
    public:
        parsings(int seed, bool _debug);
        int make_string(std::vector<int> &word);
        int concat(int label1, int label2);
        std::pair<int, int> split(int label, int position);
        bool equals(int label1, int label2);
        bool smaller(int label1, int label2);
        int longest_common_prefix(int label1, int label2);
};