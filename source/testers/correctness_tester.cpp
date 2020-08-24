#include "correctness_tester.h"

const bool debug = false;

const int ALPHABET_SIZE                 = 20;
const int RANDOM_NUMBER_OF_TEST_CASES   = 100;
const int RANDOM_NUMBER_OF_OPERATIONS   = 10000;
const int RANDOM_MAX_WORD_LENGTH        = 10000;
const int RANDOM_WORD_LENGTH            = 10;

int correctness_tester::random_integer(int lower_limit, int upper_limit) {
    return std::uniform_int_distribution<int>(lower_limit, upper_limit)(random_number_generator);
}

correctness_tester::correctness_tester(int seed) {
    random_number_generator.seed(seed);
}

std::vector<int> correctness_tester::random_word(int max_length) {
    int length = random_integer(1, max_length);
    std::vector<int> word;
    for(int i = 0; i < length; i++) {
        word.push_back(random_integer(0, ALPHABET_SIZE - 1));
    }
    return word;
}

std::pair<int, int> correctness_tester::generate_random_operations(solution *model, solution *tested, int number_of_operations) {
    std::vector<int> model_labels, tested_labels, word_lengths;
    // always start with make_string operation
    int query_type = 1, passed_tests = 0, total_tests = 0;
    for(int i = 0; i < number_of_operations; i++) {
        if(query_type == 1) {
            std::vector<int> word = random_word(RANDOM_WORD_LENGTH);
            if(debug) {
                std::cerr << "update " << (int)model_labels.size() - 1 << ": ";
                for(auto character: word) {
                    std::cerr << character << " ";
                }
                std::cerr << "\n";
            } 
            model_labels.push_back(model->make_string(word));
            tested_labels.push_back(tested->make_string(word));
            word_lengths.push_back(word.size());
        } else if(query_type == 2) {
            int index1 = random_integer(0, model_labels.size() - 1),
                index2 = random_integer(0, model_labels.size() - 1);
            if(word_lengths[index1] + word_lengths[index2] > RANDOM_MAX_WORD_LENGTH) {
                i--;
                query_type = random_integer(1, 6);
                continue;
            }
            if(debug) std::cerr << "concat " << model_labels.size() - 1 << " " << index1 << " " << index2 << "\n";
            model_labels.push_back(
                model->concat(
                    model_labels[index1],
                    model_labels[index2]
                )
            );
            tested_labels.push_back(
                tested->concat(
                    tested_labels[index1],
                    tested_labels[index2]
                )
            );
            word_lengths.push_back(word_lengths[index1] + word_lengths[index2]);
        } else if(query_type == 3) {
            int index = random_integer(0, model_labels.size() - 1);
            if(word_lengths[index] < 2) {
                i--;
                query_type = random_integer(1, 6);
                continue;
            }
            int position = random_integer(1, word_lengths[index] - 1);
            if(debug) std::cerr << "split " << model_labels.size() - 1 << " " << index << " " << position << " " << word_lengths[index] << "\n";
            auto model_output = model->split(model_labels[index], position);
            model_labels.push_back(model_output.first);
            model_labels.push_back(model_output.second);
            auto tested_output = tested->split(tested_labels[index], position);
            tested_labels.push_back(tested_output.first);
            tested_labels.push_back(tested_output.second);
            word_lengths.push_back(position);
            word_lengths.push_back(word_lengths[index] - position);
        } else if(query_type == 4) {
            int index1 = random_integer(0, model_labels.size() - 1),
                index2 = random_integer(0, model_labels.size() - 1);
            if(debug) std::cerr << "equals " << model_labels.size() - 1 << " " << index1 << " " << index2 << "\n";
            bool model_output = model->equals(
                model_labels[index1], 
                model_labels[index2]
            );
            bool tested_output = tested->equals(
                tested_labels[index1], 
                tested_labels[index2]
            );
            if(model_output == tested_output) {
                passed_tests++;
            } else if(debug) {
                std::cerr << model_output << " " << tested_output << "\n";
                return {0, 1};
            }
            total_tests++;
        } else if(query_type == 5) {
            int index1 = random_integer(0, model_labels.size() - 1),
                index2 = random_integer(0, model_labels.size() - 1);
            if(debug) std::cerr << "smaller " << model_labels.size() - 1 << " " << index1 << " " << index2 << "\n";
            bool model_output = model->smaller(
                model_labels[index1], 
                model_labels[index2]
            );
            bool tested_output = tested->smaller(
                tested_labels[index1], 
                tested_labels[index2]
            );
            if(model_output == tested_output) {
                passed_tests++;
            } else if(debug) {
                std::cerr << model_output << " " << tested_output << "\n";
                return {0, 1};
            }
            total_tests++;
        } else {
            int index1 = random_integer(0, model_labels.size() - 1),
                index2 = random_integer(0, model_labels.size() - 1);
            if(debug) std::cerr << "lcp " << model_labels.size() - 1 << " " << index1 << " " << index2 << "\n";
            int model_output = model->longest_common_prefix(
                model_labels[index1], 
                model_labels[index2]
            );
            int tested_output = tested->longest_common_prefix(
                tested_labels[index1], 
                tested_labels[index2]
            );
            if(model_output == tested_output) {
                passed_tests++;
            } else if(debug) {
                std::cerr << model_output << " " << tested_output << "\n";
                return {0, 1};
            }
            total_tests++;
        }
        query_type = random_integer(1, 6);
    }
    return {passed_tests, total_tests};
}

void correctness_tester::run_tests() {
    std::cout << "RUNNING RANDOM TESTS\n";
    std::cout << "----------------------------------------\n";
    std::cout << "RUNNING BALANCED TREES SOLUTION\n";
    int total_passed = 0, total_tests = 0, percent_done = 0;
    for(int test_case = 0; test_case < RANDOM_NUMBER_OF_TEST_CASES; test_case++) {
        std::pair<int, int> naive_random_test = generate_random_operations(
            new naive(),
            new balanced_trees(53172, 43, 1e9 + 7),
            RANDOM_NUMBER_OF_OPERATIONS
        );
        total_passed += naive_random_test.first;
        total_tests += naive_random_test.second;
        if(naive_random_test.first == 0) {
            std::cerr << test_case << "\n";
            return;
        }
        if(100 * (test_case + 1) / RANDOM_NUMBER_OF_TEST_CASES > percent_done) {
            percent_done = 100 * (test_case + 1) / RANDOM_NUMBER_OF_TEST_CASES;
            std::cout << "[";
            for(int i = 0; i < 100; i++) {
                std::cout << (i < percent_done ? "*" : " ");
            }
            std::cout << "] " << percent_done << "%" << (percent_done < 100 ? "\r" : "\n");
            std::cout.flush();
        }
    }
    std::cout << "PASSED " << total_passed << " OUT OF " << total_tests 
              << " TESTS (" << 100.0 * total_passed / total_tests << "%)\n";


    std::cout << "RUNNING RANDOM TESTS\n";
    std::cout << "----------------------------------------\n";
    std::cout << "RUNNING PARSING TREES LCP LOG^2 SOLUTION\n";
    total_passed = 0, total_tests = 0, percent_done = 0;
    for(int test_case = 0; test_case < RANDOM_NUMBER_OF_TEST_CASES; test_case++) {
        std::pair<int, int> naive_random_test = generate_random_operations(
            new naive(),
            new parsings_slow_lcp(1998, false),
            RANDOM_NUMBER_OF_OPERATIONS
        );
        total_passed += naive_random_test.first;
        total_tests += naive_random_test.second;
        if(naive_random_test.first == 0) {
            std::cerr << test_case << "\n";
            return;
        }
        if(100 * (test_case + 1) / RANDOM_NUMBER_OF_TEST_CASES > percent_done) {
            percent_done = 100 * (test_case + 1) / RANDOM_NUMBER_OF_TEST_CASES;
            std::cout << "[";
            for(int i = 0; i < 100; i++) {
                std::cout << (i < percent_done ? "*" : " ");
            }
            std::cout << "] " << percent_done << "%" << (percent_done < 100 ? "\r" : "\n");
            std::cout.flush();
        }
    }
    std::cout << "PASSED " << total_passed << " OUT OF " << total_tests 
              << " TESTS (" << 100.0 * total_passed / total_tests << "%)\n";

    std::cout << "RUNNING RANDOM TESTS\n";
    std::cout << "----------------------------------------\n";
    std::cout << "RUNNING PARSING TREES LCP LOG SOLUTION\n";
    total_passed = 0, total_tests = 0, percent_done = 0;
    for(int test_case = 0; test_case < RANDOM_NUMBER_OF_TEST_CASES; test_case++) {
        std::pair<int, int> naive_random_test = generate_random_operations(
            new naive(),
            new parsings(1998, false),
            RANDOM_NUMBER_OF_OPERATIONS
        );
        total_passed += naive_random_test.first;
        total_tests += naive_random_test.second;
        if(naive_random_test.first == 0) {
            std::cerr << test_case << "\n";
            return;
        }
        if(100 * (test_case + 1) / RANDOM_NUMBER_OF_TEST_CASES > percent_done) {
            percent_done = 100 * (test_case + 1) / RANDOM_NUMBER_OF_TEST_CASES;
            std::cout << "[";
            for(int i = 0; i < 100; i++) {
                std::cout << (i < percent_done ? "*" : " ");
            }
            std::cout << "] " << percent_done << "%" << (percent_done < 100 ? "\r" : "\n");
            std::cout.flush();
        }
    }
    std::cout << "PASSED " << total_passed << " OUT OF " << total_tests 
              << " TESTS (" << 100.0 * total_passed / total_tests << "%)\n";
}