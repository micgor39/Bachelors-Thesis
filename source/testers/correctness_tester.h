#include "../solutions/balanced_trees.h"
#include "../solutions/naive.h"
#include "../solutions/parsings.h"
#include "../solutions/parsings_slow_lcp.h"
#include <iostream>
#include <random>
#include <vector>

class correctness_tester {
  private:
    std::mt19937 random_number_generator;
    int random_integer(int lower_limit, int upper_limit);
    std::vector<int> random_word(int max_length);
    std::pair<int, int> generate_random_operations(solution *model,
                                                   solution *tested,
                                                   int number_of_test_cases);

  public:
    correctness_tester(int seed);
    void run_tests();
};