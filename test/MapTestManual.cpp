// Currently a stand alone non-automated test application
// for manual testing, debugging, and poking around with
// CLIMap. An automated MapTest will be created to unit
// (and regression?) test the following assertions:
//
//  1.  argc_caller - argc_skipped <= 0 in exec_base throws a domain error exception.
//  2.  Key-value pairs declared sooner in the CLIMap declaration take priority, and are
//      matched before, key-value pairs declared later in the CLIMap declaration.
//  3.  When a CLIMap fails to match an argument to a key, attempt to find a match in the parent map.
//  4.  Noarg keys only match on arguments passed down from above.
//  5.  Anyarg keys only match on arguments passed down from above, except for a map executed with
//      exec_main. In that case, anyarg keys match on all arguments.
//  6.  argc_main = args_handled-argc_left_or_error.
//  7.  The most recent arg handled = argv_main[argc_main-argc_left_or_error-1].
//  8.  Returning ARGMAP_EXIT_INVALID_ARG from any main results in no other
//      arguments being processed, and the invalid_arg_message argument of exec_main is printed.
//  9.  Returning ARGMAP_EXIT_SUCCESS from any main results in no other arguments being
//      processed, along with a successful exit to the system.


#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "CLIMap.hpp"

# if __cplusplus >= 201703L
    #define cpp17
#endif

using std::cout;
using std::endl;
using std::stoi;
using std::string;
using std::stringstream;

template <typename T1, typename T2>
void out_arg_res(const T1& arg, const T2& res) {
    cout << "arg = " << arg << endl;
    cout << "res = " << res << endl;
    //cout << endl;
}

int print_arg(int argc, char **argv) {
    assert(argc>0);
    cout << "print_arg" << endl;
    cout << "argc = " << argc << endl;
    out_arg_res(argv[0],argv[0]);
    return argmap_return_success(argc);
}

int print_arg_passthrough(int argc, char **argv) {
    constexpr CLIMap<> argmap {
        {anyarg, print_arg}
    };
    cout << "print_arg_passthrough" << endl;
    cout << "argc = " << argc << endl;
    auto ret = argmap.exec(argc, argv);
    cout << "exec print_arg_passthrough = " << ret << endl;
    cout << endl;
    return ret;
}

int succeed(int argc, char **argv) {
    cout << "succeed" << endl;
    cout << "argc = " << argc << endl;
    cout << endl;
    assert(argc>0);
    return argmap_return_success(argc);
}

int fail(int argc, char **argv) {
    cout << "fail" << endl;
    cout << "argc = " << argc << endl;
    cout << endl;
    assert(argc>0);
    return -1;
}

string fizzbuzz_base(int arg) {
    stringstream ss;
    if (arg%15==0) {
        ss << "fizzbuzz";
    } else if (arg%3==0) {
        ss << "fizz";
    } else if (arg%5==0) {
        ss << "buzz";
    } else {
        ss << arg;
    }
    return ss.str();
}

int fizzbuzz(int argc, char **argv) {
    assert(argc>0);
    cout << "fizzbuzz" << endl;
    cout << "argc = " << argc << endl;
    int arg = stoi(argv[0]);
    out_arg_res(arg, fizzbuzz_base(arg));
    return argmap_return_success(argc);
}

int fizzbuzz_passthrough(int argc, char **argv) {
    constexpr CLIMap<> argmap {
        {anyarg, fizzbuzz}
    };
    cout << "fizzbuzz_passthrough" << endl;
    cout << "argc = " << argc << endl;;
    auto ret = argmap.exec(argc, argv);
    cout << "exec fizzbuzz_passthrough = " << ret << endl;
    cout << endl;
    return ret;
}


template<class T>
T fib_dp(const T& n, const T& F0 = T(0), const T& F1 = T(1), bool clear_fib_map = false) {
	static std::unordered_map< T, std::unordered_map< T, std::unordered_map<T,T> > > fib_map;
    assert(n>=0);

	if (clear_fib_map) fib_map.clear();

	if (fib_map.find(F0) == fib_map.end() || fib_map.at(F0).find(F1) == fib_map.at(F0).end()) {
		fib_map[F0][F1][0] = F0;
		fib_map[F0][F1][1] = F1;
	}

	if (fib_map.at(F0).at(F1).find(n) == fib_map.at(F0).at(F1).end()) {
		return (fib_map.at(F0).at(F1)[n] = fib_dp(n-T(1), F0, F1) + fib_dp(n-T(2), F0, F1));
	}

	return fib_map.at(F0).at(F1).at(n);
}

int fib(int argc, char **argv) {
    assert(argc>0);
    cout << "fib" << endl;
    cout << "argc = " << argc << endl;
    int arg = stoi(argv[0]);
    out_arg_res(arg, fib_dp(arg));
    return argmap_return_success(argc);
}

int fib_passthrough(int argc, char ** argv) {
    constexpr CLIMap<> argmap {
        {anyarg, fib}
    };
    cout << "fib_passthrough" << endl;
    cout << "argc = " << argc << endl;
    auto ret = argmap.exec(argc, argv);
    cout << "exec fib_passthrough = " << ret << endl;
    cout << endl;
    return ret;
}

int factorial_base(int n) {
    return (n==0?1:n*factorial_base(n-1));
}

int factorial(int argc, char **argv) {
    assert(argc>0);
    cout << "factorial" << endl;
    cout << "argc = " << argc << endl;
    int arg = stoi(argv[0]);
    out_arg_res(arg, factorial_base(arg));
    return argmap_return_success(argc);
}

int factorial_passthrough(int argc, char **argv) {
    constexpr CLIMap<> argmap {
        {anyarg, factorial}
    };
    cout << "factorial_passthrough" << endl;
    cout << "argc = " << argc << endl;
    auto ret = argmap.exec(argc, argv);
    cout << "exec factorial_passthrough = " << ret << endl;
    cout << endl;
    return ret;
}

int no_argument(int argc, char **argv) {
    assert(argc>0);
    cout << "nothing to do" << endl;
    cout << "argc = " << argc << endl;
    cout << endl;
    return argmap_return_success(argc);
}

int no_match(int argc, char **argv) {
    assert(argc>0);
    cout << "invalid argument" << endl;
    cout << "arg = " << argv[0] << endl;
    cout << "argc = " << argc << endl;
    cout << endl;
    return argmap_return_success(argc);
}

int do_nothing(int argc, char **argv) {
    return argmap_return_success(argc);
}

bool up_key_fn(const char * str) { // fails on the first go, alternates therafter.
    static bool match_on_up = false;
    bool out = false;
    if (strcmp(str, "up")==0) {
        out = match_on_up;
        match_on_up = !match_on_up;
    }
    return out;
}

int main_recurse(int argc, char** argv) {
    static int depth = 0;
    constexpr CLIMap<> argmap {
        {"print", print_arg_passthrough},
        {"succeed", succeed},
        {"fail", fail},
        {"fizzbuzz", fizzbuzz_passthrough},
        {"fib", fib_passthrough},
        {"fact", factorial_passthrough},
        {up_key_fn, do_nothing},
        {"down", main_recurse},
        {noarg, no_argument}/*,
        {anyarg, no_match}*/    // That anyarg matches on downs only sabotages "up" argument.
    };

    cout << "main_recurse down" << endl;
    cout << "argc = " << argc << endl;
    cout << "depth = " << depth << endl;
    cout << endl;

    ++depth;
    auto ret = depth==1?argmap.exec_main(argc, argv):argmap.exec(argc,argv);
    --depth;

    cout << "main_recurse up " << endl;
    cout << "argc (pre) " << argc << endl;
    cout << "depth = " << depth << endl;
    cout << "ret = " << ret << endl;
    cout << endl;
    
    return ret;
}

int main(int argc, char **argv) {
    return main_recurse(argc, argv);
}

