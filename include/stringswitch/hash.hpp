#ifndef HASH_HPP_
#define HASH_HPP_
#include <string>

namespace _switch {
    template<class> struct hasher;
    template<     > struct hasher<std::string> {
        constexpr std::size_t operator()(const char *input) const {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
        }
        std::size_t operator()(const std::string& str) const {
            return (*this)(str.c_str());
        }
    };
    template<typename T>
    constexpr std::size_t hash(T&& t) {
        return hasher<typename std::decay<T>::type>()(std::forward<T>(t));
    }
    inline namespace literals {
            constexpr std::size_t operator "" _hash(const char* s, size_t) {
                return hasher<std::string>()(s);
            }
    }
}
#endif /// HASH_HPP_