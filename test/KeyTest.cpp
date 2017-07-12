#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <cstring>
#include <array>
#include <iostream>
#include <string>
#include <sstream>

class CLIMapKeyTester;

#include "CLIMap.hpp"

#if __cplusplus >= 201703L
    #define cpp17
#endif

using std::array;
using std::cout;
using std::endl;
using std::ostream;
using std::string;
using std::stringstream;

bool is_test_arg_2(const char * arg_in) {
    return (strcmp(arg_in,"test_arg_2")==0);
}

class CLIMapKeyTester {
    public:
        using ArgType = const char *;
        using KeyType = CLIMap<ArgType, bool (* const)(ArgType)>::CLIMapKey;
        bool run_standard_test() {
            constexpr KeyType key_1{"test_arg_1"};
            constexpr KeyType key_2{is_test_arg_2};
            constexpr KeyType key_3{noarg};
            constexpr KeyType key_4{anyarg};
            #ifdef cpp17
                constexpr KeyType key_5{[](ArgType arg_in){ return (strcmp(arg_in,"test_arg_5")==0); }};
            #endif

            #ifdef cpp17
                constexpr size_t N = 5;
            #else
                constexpr size_t N = 4;
            #endif

            array<const KeyType*, N> test_keys {
                 &key_1 
                ,&key_2 
                ,&key_3
                ,&key_4 
                #ifdef cpp17
                    ,&key_5
                #endif
            };

            auto match_loop = [this](ostream& out, const array<const KeyType*, N>& test_keys_param) {
                for (size_t i = 0; i != N; ++i) {
                    print_match_many(out, i+1, *test_keys_param[i], "test_arg_1", "test_arg_2", noarg, "f_~HUIX|G~"/*, anyarg*/);
                    #ifdef cpp17
                        print_match(out, i+1, 5, *test_keys_param[i], "test_arg_5");
                    #endif
                }
            };

            auto print_failure_message = [](string key_number, string arg_number) {
                cout << "Test fails on key number " << key_number << " and arg number " << arg_number << "." << endl;
            };

            stringstream ss;
            match_loop(ss, test_keys);

            bool test_passes = true;
            while (ss.eof()) {
                string key_number;
                string arg_number;
                string match;
                ss >> key_number;
                ss >> arg_number;
                ss >> match;
                if (key_number == arg_number || (key_number == "4" && arg_number != "3")) {
                    if (match != "y") {
                        test_passes = false;
                        print_failure_message(key_number, arg_number);
                    }
                } else {
                    if (match != "n") {
                        test_passes = false;
                        print_failure_message(key_number, arg_number);
                    }
                }
            }

            if (test_passes == false) {
                match_loop(cout, test_keys);
            }

            return test_passes;
        }

        bool run_match_on_anyarg_test() {
            constexpr KeyType key = {anyarg};
            bool test_passes = true;
            if(key.matches("teststr") == false) {
                test_passes = false;
            } else if (key.matches("teststr", false) == true) {
                test_passes = false;
            }
            return test_passes;
        }

    private:
        int current_arg_number = 0;

        template<typename T>
        inline void print_match(ostream& out, int key_number, int arg_number, const KeyType& key_in, T arg_in) {
            out << key_number << ' ' << arg_number << ' ' << (key_in.matches(arg_in)?'y':'n') << '\n';
        }

        inline void print_match_many(ostream&, int, const KeyType&) {
            current_arg_number = 0;
        }

        template<typename T, typename... Args>
        inline void print_match_many(ostream& out, int key_number, const KeyType& key_in, T current_arg, Args... rest_args) {
            ++current_arg_number;
            print_match(out, key_number, current_arg_number, key_in, current_arg);
            print_match_many(out, key_number, key_in, rest_args...);
        }
};

BOOST_AUTO_TEST_CASE(key_test) {
    CLIMapKeyTester tests;

    // Testing that map lookup (non-)matches correctly for match_on_anyarg = true.
    BOOST_TEST(tests.run_standard_test());

    // Testing that map lookup responds correctly to match_on_anyarg.
    BOOST_TEST(tests.run_match_on_anyarg_test());
}

/*
int main(int argc, char **argv) {
    return CLIMapKeyTester().run_test();
}
*/

