#include <cstdlib>
#include <limits>

#include "integer_tests.hpp"

using std::numeric_limits;

bool is_integer(const char * arg) {
    char *endptr;
    strtol(arg, &endptr, 10);
    return (*endptr == '\0');
}

bool is_out_of_range_integer(const char * arg) {
    char *endptr;
    long i = strtol(arg, &endptr, 10);
    return (*endptr == '\0' && (i <= numeric_limits<int>::min() || i >= numeric_limits<int>::max()));
}

bool is_positive_integer(const char * arg) {
    char *endptr;
    long i = strtol(arg, &endptr, 10);
    return (*endptr == '\0' && i > 0);
}

bool is_non_negative_integer(const char * arg) {
    char *endptr;
    long i = strtol(arg, &endptr, 10);
    return (*endptr == '\0' && i >= 0);
}

