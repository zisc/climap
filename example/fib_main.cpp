#include <iostream>
#include <stdexcept>
#include <string>

#include "CLIMap.hpp"
#include "integer_tests.hpp"
#include "whiteboard.hpp"

#include "fib_main.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::out_of_range;
using std::stoi;

int fib_main(int, char**);
int fib_f0_main(int, char**);
int fib_f1_main(int, char**);
int fib_set_f0_f1(int, char**, int*);
int fib_out_of_range_main(int, char**);
int fib_calculate_main(int, char**);
int fib_invalid_noarg_main(int, char**);
int fib_invalid_anyarg_main(int, char**);

int fib_f0 = 0;
int fib_f1 = 1;

int fib_main(int argc, char **argv) {
    constexpr CLIMap<> climap {
        {"f0", fib_f0_main},
        {"f1", fib_f1_main},
        {is_out_of_range_integer, fib_out_of_range_main},
        {is_non_negative_integer, fib_calculate_main},
        {noarg, fib_invalid_noarg_main},
        {anyarg, fib_invalid_anyarg_main}
    };
    return climap.exec(argc, argv);
}

int fib_f0_main(int argc, char **argv) {
    return fib_set_f0_f1(argc, argv, &fib_f0);
}

int fib_f1_main(int argc, char **argv) {
    return fib_set_f0_f1(argc, argv, &fib_f1);
}

int fib_set_f0_f1(int argc, char **argv, int *f0orf1Ptr) {
    assert(argc>0);

    const char * f0orf1_cstring = argv[0];

    if (argc==1) {
        cout << "No argument provided to fib/" << f0orf1_cstring << " command." << endl;
        return ARGMAP_EXIT_INVALID_ARG;
    }

    const char * arg_cstring = argv[1];
    int arg_int;
    
    try {
        arg_int = stoi(arg_cstring);
    } catch (const invalid_argument&) {
        cout << "fib/" << f0orf1_cstring << " argument \"" << arg_cstring << "\" is not an integer." << endl;
        return ARGMAP_EXIT_INVALID_ARG;
    } catch (const out_of_range&) {
        cout << "fib/" << f0orf1_cstring << " argument \"" << arg_cstring << "\" out of range. Try an integer closer to zero." << endl;
        return ARGMAP_EXIT_INVALID_ARG;
    }

    *f0orf1Ptr = arg_int;
    
    int num_args_parsed = 1;
    return argmap_return_success(argc, num_args_parsed);
}

int fib_out_of_range_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg = argv[0];
    cout << "Fib argument " << arg << " out of range. Try a non-negative integer closer to zero." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

int fib_calculate_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg_cstring = argv[0];
    int arg_int = stoi(arg_cstring);
    cout << fib(arg_int, fib_f0, fib_f1) << endl;
    return argmap_return_success(argc);
}

int fib_invalid_noarg_main(int argc, char **argv) {
    assert(argc>0);
    cout << "No argument provided to fib command." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

int fib_invalid_anyarg_main(int argc, char **argv) {
    assert(argc>0);
    cout << "Fib argument \"" << argv[1] << "\" is invalid - not \"f0\", nor \"f1\", nor a non-negative integer." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

