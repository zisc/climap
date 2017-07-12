#include <cassert>
#include <iostream>
#include <string>

#include "fact_main.hpp"
#include "fib_main.hpp"
#include "fizzbuzz_main.hpp"
#include "CLIMap.hpp"

using std::cout;
using std::endl;
using std::string;

int main(int, char**);
int whiteboard_print_help_main(int, char**);
int whiteboard_invalid_anyarg_main(int, char**);

const char * whiteboard_prog_name;

int main(int argc, char **argv) {
    constexpr CLIMap<> climap {
        {"fizzbuzz", fizzbuzz_main},
        {"fact", fact_main},
        {"fib", fib_main},
        {"help", whiteboard_print_help_main},
        {noarg, whiteboard_print_help_main},
        {anyarg, whiteboard_invalid_anyarg_main}
    };
    whiteboard_prog_name = argv[0];
    const string invalid_arg_message = string("Run \"") + whiteboard_prog_name + " help\" for more information.\n";
    return climap.exec_main(argc, argv, invalid_arg_message);
}

int whiteboard_print_help_main(int argc, char **argv) {
    assert(argc>0);
    cout << whiteboard_prog_name << "\n"
            "   fizzbuzz <n>    For some positive (>=1) integer n.\n"
            "   fact <n>        Factorial of some non-negative integer n (n!).\n"
            "   fib             Fibonacci series whereby fibonacci(n) = fibonacci(n-1) + fibonacci(n-2)\n"
            "       f0 <z>      Set fibonacci(0), the zeroth number in the fibonacci series, to some integer z. Set to 0 by default.\n"
            "       f1 <z>      Set fibonacci(1), the first number in the fibonacci series, to some integer z. Set to 1 by default.\n"
            "       <n>         Find fibonacci(n), the nth number in the fibonacci series, for some non-negative integer n.\n";

    return argmap_return_success(argc);
}

int whiteboard_invalid_anyarg_main(int argc, char **argv) {
    assert(argc>0);
    const char * arg = argv[0];
    cout << "Invalid argument \"" << arg << "\"." << endl;
    return ARGMAP_EXIT_INVALID_ARG;
}

