#include "../solutions/balanced_trees.h"
#include "../solutions/parsings.h"
#include "../solutions/parsings_slow_lcp.h"
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
using namespace std::chrono;

typedef time_point<high_resolution_clock> timepoint;

class speed_tester {
  private:
    const bool debug = false;
    const int ALPHABET_SIZE = 2;
    const int NUMBER_OF_TEST_CASES = 10;
    const int RANDOM_NUMBER_OF_OPERATIONS = 1e5;
    const int RANDOM_MAX_WORD_LENGTH = 1e6;
    const int RANDOM_WORD_LENGTH = 1e3;
    const int SUFFIX_ARRAY_LENGTH = 1e4;
    const int SUFFIX_ARRAY_QUERIES = 1e5;
    int test_type_number;
    std::mt19937 random_number_generator;
    long long calculate_duration(timepoint start_tp, timepoint stop_tp);
    int random_integer(int lower_limit, int upper_limit);
    std::vector<int> random_word(int max_length, bool draw_length);
    long long generate_random_operations(solution *test,
                                         int number_of_test_cases);
    long long suffix_array_test(solution *test, int word_length, int lcp_tests);

  public:
    speed_tester(int seed, int _test_type_number);
    void run_tests();
};