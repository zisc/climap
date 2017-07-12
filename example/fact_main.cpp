#include <cassert>
#include <iostream>
#include <string>

#include "CLIMap.hpp"
#include "integer_tests.hpp"
#include "whiteboard.hpp"

#include "fact_main.hpp"

using std::cout;
using std::endl;
using std::stoi;

int fact_main(int, char**);
int fact_out_of_range_main(int, char**);
int fact_calculate_main(int, char**);
int fact_invalid_noarg_main(int, char**);
int fact_invalid_anyarg_main(int, char**);

int fact_main(int argc, char **argv) {
    assert(argc>0);
    constexpr CLIMap<> climap {
        {is_out_of_range_integer, fact_out_of_range_main},
        {is_non_negative_integer, fact_calculate_main},
        {noarg, fact_invalid_noarg_main},
        {anyarg, fact_invalid_anyarg_main}
    };
    return climap.exec(argc, argv);
}

int fact_out_of_range_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg = argv[0];
    cout << "Fact argument " << arg << " out of range. Try a non-negative integer closer to zero." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

int fact_calculate_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg_cstring = argv[0];
    int arg_int = stoi(arg_cstring);
    cout << fact(arg_int) << endl;
    return argmap_return_success(argc);
}

int fact_invalid_noarg_main(int argc, char **argv) {
    assert(argc>0);
    cout << "No argument provided to fact command." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

int fact_invalid_anyarg_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg = argv[0];
    cout << "Fact argument " << arg << " is not a non-negative integer." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

