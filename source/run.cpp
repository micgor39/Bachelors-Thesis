#include "testers/correctness_tester.h"
#include <iostream>

int main(int argc, char **argv) {
    if(argc != 2) {
        std::cerr << "usage: ./run seed" << "\n";
        return 1;
    }
    int seed = atoi(argv[1]);
    correctness_tester correctness_tester(seed);
    correctness_tester.run_tests();
    return 0;
}