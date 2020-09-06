#include "../solutions/balanced_trees.h"
#include "../solutions/naive.h"
#include "../solutions/parsings.h"
#include "../solutions/parsings_slow_lcp.h"
#include <iostream>
#include <random>
#include <vector>

class correctness_tester {
  private:
    const bool debug = false;
    const int ALPHABET_SIZE = 2;
    const int NUMBER_OF_TEST_CASES = 1000;
    const int RANDOM_NUMBER_OF_OPERATIONS = 10000;
    const int RANDOM_MAX_WORD_LENGTH = 10000;
    const int RANDOM_WORD_LENGTH = 10;
    const int SUFFIX_ARRAY_LENGTH = 1000;
    const int SUFFIX_ARRAY_QUERIES = 1000;
    int test_type_number;
    std::mt19937 random_number_generator;
    int random_integer(int lower_limit, int upper_limit);
    std::vector<int> random_word(int max_length, bool draw_length);
    std::pair<int, int> generate_random_operations(solution *model,
                                                   solution *test,
                                                   int number_of_test_cases);
    std::pair<int, int> suffix_array_test(solution *model, solution *test,
                                          int word_length, int lcp_tests);

  public:
    correctness_tester(int seed, int _test_type_number);
    void run_tests();
};