#ifndef CLIMAP_HEADER_GUARD
#define CLIMAP_HEADER_GUARD

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

struct CLIMapDT;    // CLIMapDefaultTemplates

template<typename T>
struct remove_elem_const {
    friend class CLIMapDT;
    static_assert(
         std::is_same<T,const char>::value || std::is_same<T,const char *>::value,
        "remove_elem_const implmented for const char * and const char only, please define ArgIterator template."
   );
};

template<>
struct remove_elem_const<const char> {
    friend class CLIMapDT;
    private:
        using type = char;
};

template<>
struct remove_elem_const<const char *> {
    friend class CLIMapDT;
    private:
        using type = char *;
};

struct CLIMapDT {   // CLIMapDefaultTemplates
    using ArgType = const char *;
    using MatchFnType = bool (* const)(ArgType);
    using ArgIterator = remove_elem_const<ArgType>::type*;
};

class NoArgType {};
class AnyArgType {};

const NoArgType noarg{};
const AnyArgType anyarg{};

constexpr int argmap_return_success(int argc, int args_parsed = 0) { return argc - (1+args_parsed); } // Returns argc for the next argument.
constexpr int ARGMAP_EXIT_INVALID_ARG = std::numeric_limits<int>::max();
constexpr int ARGMAP_EXIT_SUCCESS = 0;

template <typename ArgType = CLIMapDT::ArgType, typename MatchFnType = CLIMapDT::MatchFnType, typename ArgIterator = CLIMapDT::ArgIterator>
class CLIMap {
    // Yet to be automatically unit (and regression?) tested. See test/MapTestManual for a manual testing application.
    friend class CLIMapKeyTester;

    private:
        class CLIMapKey;

        using RawPairType = std::pair<CLIMapKey, int (*)(int, ArgIterator)>;
        using RawMapType = std::initializer_list<RawPairType>;
        using RawMapCIter = typename RawMapType::const_iterator;

        RawMapType raw_map;

        class RawPairPredArgFtor {  // Pred for find_if, created so that arg can be captured by const reference.
            public:
                RawPairPredArgFtor(const ArgType& arg_in): arg_cref{arg_in}, match_on_anyarg{true}  { };
                RawPairPredArgFtor(const ArgType& arg_in, bool match_on_anyarg_in): arg_cref{arg_in}, match_on_anyarg{match_on_anyarg_in} { };

                bool operator()(const RawPairType& raw_pair) {
                    return raw_pair.first.matches(arg_cref, match_on_anyarg);
                }

            private:
                const ArgType& arg_cref;
                bool match_on_anyarg;
        };

        RawMapCIter get_match_iter(const ArgType& arg, bool match_on_anyarg = true) const {
            auto raw_map_cbegin = static_cast<RawMapCIter>(raw_map.begin());
            auto raw_map_cend = static_cast<RawMapCIter>(raw_map.end());
            RawPairPredArgFtor pred(arg, match_on_anyarg);
            return std::find_if(raw_map_cbegin, raw_map_cend, pred);
        }

        RawMapCIter get_match_iter(NoArgType) const {
            auto raw_map_cbegin = static_cast<RawMapCIter>(raw_map.begin());
            auto raw_map_cend = static_cast<RawMapCIter>(raw_map.end());
            auto pred = [](const RawPairType& raw_pair) { return raw_pair.first.matches(noarg); };
            return std::find_if(raw_map_cbegin, raw_map_cend, pred);
        }
        
        int exec_base(int argc_caller, ArgIterator argv_caller, int args_to_skip, bool match_on_anyarg_in_loop) const {
             // Will need bulk testing.
            int argc_left_or_error = -1;

            if (argc_caller - args_to_skip > 0) {   // argc_caller - args_to_skip <= 0 is domain error, see else.
                int argc_callee;
                ArgIterator argv_callee;
                RawMapCIter match_iter;
                
                if (argc_caller - args_to_skip == 1) {  // argv_caller[0] is the argument that triggered the calling function.
                    argc_callee = 1;                    // argc_caller - args_to_skip == 1 means there are no arguments for callee, bar maybe noarg.
                    argv_callee = argv_caller;
                    std::advance(argv_callee, args_to_skip);
                    match_iter = get_match_iter(noarg);
                } else {
                    argc_callee = argc_caller - (1+args_to_skip);
                    argv_callee = argv_caller;
                    std::advance(argv_callee, 1+args_to_skip);
                    auto arg = *argv_callee;
                    match_iter = get_match_iter(arg, true);
                }

                argc_left_or_error = argc_callee;   // In case arg has no matching handler function. 

                while (match_iter != static_cast<RawMapCIter>(raw_map.end())) {
                    // Call function matched to the next argument to parse and break if unsuccessful or there are no arguments left to parse.
                    argc_left_or_error = match_iter->second(argc_callee, argv_callee);
                    if (argc_left_or_error <= 0 || argc_left_or_error == ARGMAP_EXIT_INVALID_ARG) break;

                    // Find the next argument to parse (and associated argc), look it up in raw_map and break if no match found.
                    auto number_of_args_handled = argc_callee - argc_left_or_error; // This is why argc_callee and argc_left_or_error are different variables.
                    std::advance(argv_callee, number_of_args_handled);
                    argc_callee = argc_left_or_error;
                    match_iter = get_match_iter(*argv_callee, match_on_anyarg_in_loop);
                }
                // Loop exits with:
                //      * argc_left_or_error up-to-date, which is then returned.
                //      * match_iter indicating no-match (2nd break), or pointing to the pair for the last handling function (value) executed (1st break).

            } else {
                throw std::domain_error(
                        "CLIMap::exec called with argc_caller - args_to_skip <=0: "
                        "argc_caller = " + std::to_string(argc_caller) + \
                        ", args_to_skip = " + std::to_string(args_to_skip) + \
                        "."
                );
            }

            return argc_left_or_error;

        }


    public:
        constexpr CLIMap(std::initializer_list<CLIMap<ArgType, MatchFnType, ArgIterator>::RawPairType> init_list): raw_map(init_list) { }

        int exec(int argc_caller, ArgIterator argv_caller, int args_to_skip = 0) const {
            bool match_on_anyarg_in_loop = false;
            return exec_base(argc_caller, argv_caller, args_to_skip, match_on_anyarg_in_loop);
        }

        int exec_main(int argc_caller, ArgIterator argv_caller) const {
            int args_to_skip = 0;
            bool match_on_anyarg_in_loop = true;
            return exec_base(argc_caller, argv_caller, args_to_skip, match_on_anyarg_in_loop);
        }

        int exec_main(int argc_caller, ArgIterator argv_caller, int args_to_skip) const {
            bool match_on_anyarg_in_loop = true;
            return exec_base(argc_caller, argv_caller, args_to_skip, match_on_anyarg_in_loop);
        }

        template<typename MsgType>
        int exec_main(int argc_caller, ArgIterator argv_caller, const MsgType& invalid_arg_message, int args_to_skip = 0) const {
            bool match_on_anyarg_in_loop = true;
            int argc_left_or_error = exec_base(argc_caller, argv_caller, args_to_skip, match_on_anyarg_in_loop);
            if (argc_left_or_error == ARGMAP_EXIT_INVALID_ARG) {
                std::cout << invalid_arg_message;
            } else if (argc_left_or_error > 0) {
                int unrecognised_arg_index = argc_caller - argc_left_or_error;
                ArgIterator unrecognised_arg_iter = argv_caller;
                std::advance(unrecognised_arg_iter, unrecognised_arg_index);
                std::cout << "Argument number " << unrecognised_arg_index << " (\"" << *unrecognised_arg_iter << "\") unrecognised." << std::endl;
                std::cout << invalid_arg_message;
            }
            return argc_left_or_error;
        }

};


template<typename ArgType, typename MatchFnType, typename ArgIterator>
class CLIMap<ArgType, MatchFnType, ArgIterator>::CLIMapKey {
    public:
        constexpr CLIMapKey(const ArgType& arg): key{.raw_arg=arg}, key_type{RawKeyType::raw_arg} { }

        constexpr CLIMapKey(const MatchFnType& fn): key{.matching_function=fn}, key_type{RawKeyType::matching_function} { }

        constexpr CLIMapKey(const AnyArgType& aa): key{.any_arg=aa}, key_type{RawKeyType::any_arg} { }

        constexpr CLIMapKey(const NoArgType& na): key{.no_arg = na}, key_type{RawKeyType::no_arg} { }
        
        bool matches(const ArgType& arg, bool match_on_anyarg = true) const {
            bool is_match = false;
            if (key_type == RawKeyType::raw_arg) {
                is_match = arg_equality(key.raw_arg, arg);
            } else if (key_type == RawKeyType::matching_function) {
                is_match = key.matching_function(arg);
            } else if (key_type == RawKeyType::no_arg) {
                is_match = false;
            } else if (key_type == RawKeyType::any_arg) {
                is_match = match_on_anyarg;
            } else {
                std::cerr << "ArgsMapKey.key_type not initialised." << std::endl;
                assert(false);
            }
            return is_match;
        }

        bool matches(NoArgType) const {
            return key_type==RawKeyType::no_arg;
        }

        bool matches(AnyArgType) const {
            return key_type==RawKeyType::any_arg;
        }

    private:
        union RawKey {
            ArgType raw_arg;
            MatchFnType matching_function;
            AnyArgType any_arg;
            NoArgType no_arg;
        };

        enum class RawKeyType {raw_arg, matching_function, any_arg, no_arg};

        const RawKey key;
        const RawKeyType key_type;

        template<typename T> 
        bool arg_equality(const T& arg1, const T& arg2) const noexcept(noexcept(operator==(arg1,arg2))) {
            return arg1==arg2;
        }

        template<typename T, std::size_t M, std::size_t N>
        bool arg_equality(const T (& arg1)[M], const T (& arg2)[N]) const noexcept(noexcept(arg_equality(*arg1,*arg2))) {
            static_assert(M==N, "ArgsMap::key array size is not equal to corresponding ArgsMap::matches argument array size.");
            for (std::size_t i = 0; i != N; ++i) {
                if (!arg_equality(arg1[i], arg2[i])) {
                    return false;
                }
            }
            return true;
        }

        bool arg_equality(char const * const arg1, char const * const arg2) const noexcept(noexcept(std::strcmp(arg1, arg2))) {
            return std::strcmp(arg1, arg2)==0;
        }
};

#endif
