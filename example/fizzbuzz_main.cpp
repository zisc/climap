#include <cassert>
#include <iostream>
#include <string>

#include "CLIMap.hpp"
#include "integer_tests.hpp"
#include "whiteboard.hpp"

#include "fizzbuzz_main.hpp"

using std::cout;
using std::endl;
using std::stoi;

int fizzbuzz_main(int, char**);
int fizzbuzz_out_of_range_main(int, char**);
int fizzbuzz_calculate_main(int, char**);
int fizzbuzz_invalid_noarg_main(int, char**);
int fizzbuzz_invalid_anyarg_main(int, char**);

int fizzbuzz_main(int argc, char **argv) {
    assert(argc>0);
    constexpr CLIMap<> climap {
        {is_out_of_range_integer, fizzbuzz_out_of_range_main},
        {is_positive_integer, fizzbuzz_calculate_main},
        {noarg, fizzbuzz_invalid_noarg_main},
        {anyarg, fizzbuzz_invalid_anyarg_main}
    };
    return climap.exec(argc, argv);
}

int fizzbuzz_out_of_range_main(int argc, char **argv) {
    const char * arg = argv[0];
    cout << "Fizzbuzz argument " << arg << " out of range. Try a positive integer closer to zero." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

int fizzbuzz_calculate_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg_cstring = argv[0];
    int arg_int = stoi(arg_cstring);
    cout << fizzbuzz(arg_int) << endl;
    return argmap_return_success(argc);
}

int fizzbuzz_invalid_noarg_main(int argc, char **argv) {
    assert(argc>0);
    cout << "No argument provided to fizzbuzz command." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

int fizzbuzz_invalid_anyarg_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg = argv[0];
    cout << "Fizzbuzz argument " << arg << " is not a positive (>=1) integer." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

