#include <stdexcept>
#include <string>

#include "whiteboard.hpp"

using std::invalid_argument;
using std::string;
using std::to_string;

string fizzbuzz(int n) {
    if (n<1) throw invalid_argument("The fizzbuzz function accepts only positive (>=1) integer input, called with n = " + to_string(n) + ".");
    
    std::string ret;
    if (n%15==0) {
        ret = "fizzbuzz";
    } else if (n%3==0) {
        ret = "fizz";
    } else if (n%5==0) {
        ret = "buzz";
    } else {
        ret = to_string(n);
    }

    return ret;
}

int fact(int n) {
    // Returns n factorial (n!).
    if (n < 0) throw invalid_argument("fact function accepts only non-negative integer input, called with n = " + to_string(n) + ".");
    return (n==0?1:n*fact(n-1));
}

int fib(int n, int f0, int f1) {
    // Returns the nth fibonacci number.

    if (n<0) throw invalid_argument("fib function accepts only non-negative integer input, called with n = " + to_string(n) + ".");

    if (n==0) return f0;
    if (n==1) return f1;

    int fnm2 = 0;
    int fnm1 = f0;
    int fn = f1;

    for (int i = 2; i <= n; ++i) {
        fnm2 = fnm1;
        fnm1 = fn;
        fn = fnm1 + fnm2;
    }

    return fn;
}

