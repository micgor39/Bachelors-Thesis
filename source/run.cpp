#include "testers/correctness_tester.h"
#include "testers/speed_tester.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "usage: ./run seed correctness/speed test_type_number"
                  << "\n";
        return 1;
    }
    int seed = atoi(argv[1]);
    std::string type = argv[2];
    int test_type_number = atoi(argv[3]);
    if (type == "correctness") {
        correctness_tester correctness_tester(seed, test_type_number);
        correctness_tester.run_tests();
    } else if (type == "speed") {
        speed_tester speed_tester(seed, test_type_number);
        speed_tester.run_tests();
    }
    return 0;
}