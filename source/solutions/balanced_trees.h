#include "solution.h"
#include <vector>
#include <random>
#include <iostream>

class balanced_trees: public solution {
    private:
        struct treap {
            long long priority;
            int character;
            int leftmost_character;
            int size;
            int hash;
            int base_power;
            treap* left_subtree;
            treap* right_subtree;
            ~treap();
        };
        int base, modulo;
        std::mt19937_64 random_number_generator;
        std::vector<treap*> stream;
        long long random_long_long();
        int get_size(treap* t);
        int get_hash(treap *t);
        int get_base_power(treap *t);
        void update_values(treap* t);
        treap* create(int character);
        treap* create(std::vector<int> &word);
        treap* merge(treap *t1, treap *t2);
        std::pair<treap*, treap*> split(treap* t, int left_tree_size);
        void print_treap(treap* t);
    public:
        balanced_trees(int seed, int _base, int _modulo);
        ~balanced_trees();
        int make_string(std::vector<int> &word);
        int concat(int label1, int label2);
        std::pair<int, int> split(int label, int position);
        bool equals(int label1, int label2);
        bool smaller(int label1, int label2);
        int longest_common_prefix(int label1, int label2);
};