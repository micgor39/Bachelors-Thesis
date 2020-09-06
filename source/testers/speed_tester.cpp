#include "speed_tester.h"

long long speed_tester::calculate_duration(timepoint start_tp,
                                           timepoint stop_tp) {
    return duration_cast<microseconds>(stop_tp - start_tp).count();
}

int speed_tester::random_integer(int lower_limit, int upper_limit) {
    return std::uniform_int_distribution<int>(lower_limit, upper_limit)(
        random_number_generator);
}

speed_tester::speed_tester(int seed, int _test_type_number) {
    random_number_generator.seed(seed);
    test_type_number = _test_type_number;
}

std::vector<int> speed_tester::random_word(int max_length, bool draw_length) {
    int length = draw_length ? random_integer(1, max_length) : max_length;
    std::vector<int> word;
    for (int i = 0; i < length; i++) {
        word.push_back(random_integer(0, ALPHABET_SIZE - 1));
    }
    return word;
}

long long speed_tester::generate_random_operations(solution *test,
                                                   int number_of_operations) {
    std::vector<int> test_labels, word_lengths;
    // always start with make_string operation
    long long total_time_ms = 0;
    int query_type = 1;
    for (int i = 0; i < number_of_operations; i++) {
        if (query_type == 1) {
            std::vector<int> word = random_word(RANDOM_WORD_LENGTH, true);
            auto start_tp = high_resolution_clock::now();
            test_labels.push_back(test->make_string(word));
            auto stop_tp = high_resolution_clock::now();
            total_time_ms += calculate_duration(start_tp, stop_tp);
            word_lengths.push_back(word.size());
        } else if (query_type == 2) {
            int index1 = random_integer(0, test_labels.size() - 1),
                index2 = random_integer(0, test_labels.size() - 1);
            if (word_lengths[index1] + word_lengths[index2] >
                RANDOM_MAX_WORD_LENGTH) {
                i--;
                query_type = random_integer(1, 6);
                continue;
            }
            auto start_tp = high_resolution_clock::now();
            test_labels.push_back(
                test->concat(test_labels[index1], test_labels[index2]));
            auto stop_tp = high_resolution_clock::now();
            total_time_ms += calculate_duration(start_tp, stop_tp);
            word_lengths.push_back(word_lengths[index1] + word_lengths[index2]);
        } else if (query_type == 3) {
            int index = random_integer(0, test_labels.size() - 1);
            if (word_lengths[index] < 2) {
                i--;
                query_type = random_integer(1, 6);
                continue;
            }
            int position = random_integer(1, word_lengths[index] - 1);
            auto start_tp = high_resolution_clock::now();
            auto test_output = test->split(test_labels[index], position);
            auto stop_tp = high_resolution_clock::now();
            total_time_ms += calculate_duration(start_tp, stop_tp);
            test_labels.push_back(test_output.first);
            test_labels.push_back(test_output.second);
            word_lengths.push_back(position);
            word_lengths.push_back(word_lengths[index] - position);
        } else if (query_type == 4) {
            int index1 = random_integer(0, test_labels.size() - 1),
                index2 = random_integer(0, test_labels.size() - 1);
            auto start_tp = high_resolution_clock::now();
            bool test_output =
                test->equals(test_labels[index1], test_labels[index2]);
            auto stop_tp = high_resolution_clock::now();
            total_time_ms += calculate_duration(start_tp, stop_tp);
        } else if (query_type == 5) {
            int index1 = random_integer(0, test_labels.size() - 1),
                index2 = random_integer(0, test_labels.size() - 1);
            auto start_tp = high_resolution_clock::now();
            bool test_output =
                test->smaller(test_labels[index1], test_labels[index2]);
            auto stop_tp = high_resolution_clock::now();
            total_time_ms += calculate_duration(start_tp, stop_tp);
        } else {
            int index1 = random_integer(0, test_labels.size() - 1),
                index2 = random_integer(0, test_labels.size() - 1);
            auto start_tp = high_resolution_clock::now();
            int test_output = test->longest_common_prefix(test_labels[index1],
                                                          test_labels[index2]);
            auto stop_tp = high_resolution_clock::now();
            total_time_ms += calculate_duration(start_tp, stop_tp);
        }
        query_type = random_integer(1, 6);
    }
    delete test;
    test_labels.clear();
    test_labels.shrink_to_fit();
    word_lengths.clear();
    word_lengths.shrink_to_fit();
    return total_time_ms;
}

long long speed_tester::suffix_array_test(solution *test, int word_length,
                                          int lcp_tests) {
    long long total_time_ms = 0;
    std::vector<int> word = random_word(word_length, false),
                     first_prefix = {word[0]};
    auto start_tp = high_resolution_clock::now();
    std::vector<int> test_labels = {test->make_string(first_prefix)};
    auto stop_tp = high_resolution_clock::now();
    total_time_ms += calculate_duration(start_tp, stop_tp);
    std::vector<int> single_letter_word;
    for (int i = 1; i < word_length; i++) {
        single_letter_word = {word[i]};
        start_tp = high_resolution_clock::now();
        test_labels.push_back(test->concat(
            test_labels[i - 1], test->make_string(single_letter_word)));
        stop_tp = high_resolution_clock::now();
        total_time_ms += calculate_duration(start_tp, stop_tp);
    }
    for (int i = 0; i < lcp_tests; i++) {
        int index1 = random_integer(0, word_length - 1),
            index2 = random_integer(0, word_length - 1);
        start_tp = high_resolution_clock::now();
        int test_output = test->longest_common_prefix(test_labels[index1],
                                                      test_labels[index2]);
        stop_tp = high_resolution_clock::now();
        total_time_ms += calculate_duration(start_tp, stop_tp);
    }
    delete test;
    test_labels.clear();
    test_labels.shrink_to_fit();
    word.clear();
    word.shrink_to_fit();
    return total_time_ms;
}

void speed_tester::run_tests() {

    long long total_time_ms, minimum_time_ms, maximum_time_ms;
    int percent_done;

    if (test_type_number == 1) {

        std::cout << "RUNNING SPEED RANDOM TESTS\n";
        std::cout << "----------------------------------------\n";
        std::cout << "RUNNING PARSE TREES LCP LOG SOLUTION\n";
        total_time_ms = 0, minimum_time_ms = 1e9, maximum_time_ms = 0,
        percent_done = 0;
        for (int test_case = 0; test_case < NUMBER_OF_TEST_CASES; test_case++) {
            long long time_ms = generate_random_operations(
                new parsings(1998, false), RANDOM_NUMBER_OF_OPERATIONS);
            total_time_ms += time_ms;
            maximum_time_ms = std::max(maximum_time_ms, time_ms);
            minimum_time_ms = std::min(minimum_time_ms, time_ms);
            if (100 * (test_case + 1) / NUMBER_OF_TEST_CASES > percent_done) {
                percent_done = 100 * (test_case + 1) / NUMBER_OF_TEST_CASES;
                std::cout << "[";
                for (int i = 0; i < 100; i++) {
                    std::cout << (i < percent_done ? "*" : " ");
                }
                std::cout << "] " << percent_done << "%"
                          << (percent_done < 100 ? "\r" : "\n");
                std::cout.flush();
            }
        }
        std::cout << "AVERAGE TIME: "
                  << total_time_ms / NUMBER_OF_TEST_CASES / 1000
                  << "ms MINIMUM TIME: " << minimum_time_ms / 1000
                  << "ms MAXIMUM TIME: " << maximum_time_ms / 1000 << "ms\n";

        std::cout << "RUNNING SPEED RANDOM TESTS\n";
        std::cout << "----------------------------------------\n";
        std::cout << "RUNNING BALANCED TREES SOLUTION\n";
        total_time_ms = 0, minimum_time_ms = 1e9, maximum_time_ms = 0,
        percent_done = 0;
        for (int test_case = 0; test_case < NUMBER_OF_TEST_CASES; test_case++) {
            long long time_ms = generate_random_operations(
                new balanced_trees(53172, 43, 1e9 + 7),
                RANDOM_NUMBER_OF_OPERATIONS);
            total_time_ms += time_ms;
            maximum_time_ms = std::max(maximum_time_ms, time_ms);
            minimum_time_ms = std::min(minimum_time_ms, time_ms);
            if (100 * (test_case + 1) / NUMBER_OF_TEST_CASES > percent_done) {
                percent_done = 100 * (test_case + 1) / NUMBER_OF_TEST_CASES;
                std::cout << "[";
                for (int i = 0; i < 100; i++) {
                    std::cout << (i < percent_done ? "*" : " ");
                }
                std::cout << "] " << percent_done << "%"
                          << (percent_done < 100 ? "\r" : "\n");
                std::cout.flush();
            }
        }
        std::cout << "AVERAGE TIME: "
                  << total_time_ms / NUMBER_OF_TEST_CASES / 1000
                  << "ms MINIMUM TIME: " << minimum_time_ms / 1000
                  << "ms MAXIMUM TIME: " << maximum_time_ms / 1000 << "ms\n";

        std::cout << "RUNNING SPEED RANDOM TESTS\n";
        std::cout << "----------------------------------------\n";
        std::cout << "RUNNING PARSE TREES LCP LOG^2 SOLUTION\n";
        total_time_ms = 0, minimum_time_ms = 1e9, maximum_time_ms = 0,
        percent_done = 0;
        for (int test_case = 0; test_case < NUMBER_OF_TEST_CASES; test_case++) {
            long long time_ms =
                generate_random_operations(new parsings_slow_lcp(1998, false),
                                           RANDOM_NUMBER_OF_OPERATIONS);
            total_time_ms += time_ms;
            maximum_time_ms = std::max(maximum_time_ms, time_ms);
            minimum_time_ms = std::min(minimum_time_ms, time_ms);
            if (100 * (test_case + 1) / NUMBER_OF_TEST_CASES > percent_done) {
                percent_done = 100 * (test_case + 1) / NUMBER_OF_TEST_CASES;
                std::cout << "[";
                for (int i = 0; i < 100; i++) {
                    std::cout << (i < percent_done ? "*" : " ");
                }
                std::cout << "] " << percent_done << "%"
                          << (percent_done < 100 ? "\r" : "\n");
                std::cout.flush();
            }
        }
        std::cout << "AVERAGE TIME: "
                  << total_time_ms / NUMBER_OF_TEST_CASES / 1000
                  << "ms MINIMUM TIME: " << minimum_time_ms / 1000
                  << "ms MAXIMUM TIME: " << maximum_time_ms / 1000 << "ms\n";

    } else if (test_type_number == 2) {

        std::cout << "RUNNING SPEED SUFFIX ARRAY TESTS\n";
        std::cout << "----------------------------------------\n";
        std::cout << "RUNNING BALANCED TREES SOLUTION\n";
        total_time_ms = 0, minimum_time_ms = 1e9, maximum_time_ms = 0,
        percent_done = 0;
        for (int test_case = 0; test_case < NUMBER_OF_TEST_CASES; test_case++) {
            long long time_ms =
                suffix_array_test(new balanced_trees(53172, 43, 1e9 + 7),
                                  SUFFIX_ARRAY_LENGTH, SUFFIX_ARRAY_QUERIES);
            total_time_ms += time_ms;
            maximum_time_ms = std::max(maximum_time_ms, time_ms);
            minimum_time_ms = std::min(minimum_time_ms, time_ms);
            if (100 * (test_case + 1) / NUMBER_OF_TEST_CASES > percent_done) {
                percent_done = 100 * (test_case + 1) / NUMBER_OF_TEST_CASES;
                std::cout << "[";
                for (int i = 0; i < 100; i++) {
                    std::cout << (i < percent_done ? "*" : " ");
                }
                std::cout << "] " << percent_done << "%"
                          << (percent_done < 100 ? "\r" : "\n");
                std::cout.flush();
            }
        }
        std::cout << "AVERAGE TIME: "
                  << total_time_ms / NUMBER_OF_TEST_CASES / 1000
                  << "ms MINIMUM TIME: " << minimum_time_ms / 1000
                  << "ms MAXIMUM TIME: " << maximum_time_ms / 1000 << "ms\n";

        std::cout << "RUNNING SPEED SUFFIX ARRAY TESTS\n";
        std::cout << "----------------------------------------\n";
        std::cout << "RUNNING PARSE TREES LCP LOG SOLUTION\n";
        total_time_ms = 0, minimum_time_ms = 1e9, maximum_time_ms = 0,
        percent_done = 0;
        for (int test_case = 0; test_case < NUMBER_OF_TEST_CASES; test_case++) {
            long long time_ms =
                suffix_array_test(new parsings(1998, false),
                                  SUFFIX_ARRAY_LENGTH, SUFFIX_ARRAY_QUERIES);
            total_time_ms += time_ms;
            maximum_time_ms = std::max(maximum_time_ms, time_ms);
            minimum_time_ms = std::min(minimum_time_ms, time_ms);
            if (100 * (test_case + 1) / NUMBER_OF_TEST_CASES > percent_done) {
                percent_done = 100 * (test_case + 1) / NUMBER_OF_TEST_CASES;
                std::cout << "[";
                for (int i = 0; i < 100; i++) {
                    std::cout << (i < percent_done ? "*" : " ");
                }
                std::cout << "] " << percent_done << "%"
                          << (percent_done < 100 ? "\r" : "\n");
                std::cout.flush();
            }
        }
        std::cout << "AVERAGE TIME: "
                  << total_time_ms / NUMBER_OF_TEST_CASES / 1000
                  << "ms MINIMUM TIME: " << minimum_time_ms / 1000
                  << "ms MAXIMUM TIME: " << maximum_time_ms / 1000 << "ms\n";

        std::cout << "RUNNING SPEED SUFFIX ARRAY TESTS\n";
        std::cout << "----------------------------------------\n";
        std::cout << "RUNNING PARSE TREES LCP LOG^2 SOLUTION\n";
        total_time_ms = 0, minimum_time_ms = 1e9, maximum_time_ms = 0,
        percent_done = 0;
        for (int test_case = 0; test_case < NUMBER_OF_TEST_CASES; test_case++) {
            long long time_ms =
                suffix_array_test(new parsings_slow_lcp(1998, false),
                                  SUFFIX_ARRAY_LENGTH, SUFFIX_ARRAY_QUERIES);
            total_time_ms += time_ms;
            maximum_time_ms = std::max(maximum_time_ms, time_ms);
            minimum_time_ms = std::min(minimum_time_ms, time_ms);
            if (100 * (test_case + 1) / NUMBER_OF_TEST_CASES > percent_done) {
                percent_done = 100 * (test_case + 1) / NUMBER_OF_TEST_CASES;
                std::cout << "[";
                for (int i = 0; i < 100; i++) {
                    std::cout << (i < percent_done ? "*" : " ");
                }
                std::cout << "] " << percent_done << "%"
                          << (percent_done < 100 ? "\r" : "\n");
                std::cout.flush();
            }
        }
        std::cout << "AVERAGE TIME: "
                  << total_time_ms / NUMBER_OF_TEST_CASES / 1000
                  << "ms MINIMUM TIME: " << minimum_time_ms / 1000
                  << "ms MAXIMUM TIME: " << maximum_time_ms / 1000 << "ms\n";
    }
}