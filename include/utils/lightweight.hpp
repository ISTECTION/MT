#ifndef _LIGHTWEIGHT_HPP
#define _LIGHTWEIGHT_HPP
#include <sstream>
#include <cctype>
#include <string>

inline void ltrim(std::string& _out) { _out.erase(0, _out.find_first_not_of(" \t\n")); }
inline void rtrim(std::string& _out) { _out.erase(_out.find_last_not_of(" \t\n") + 1); }

/// Возможен тут будет баг (написал на коленке в 1:08)
bool not_spaces (const std::string& _line) {
    for (const auto& _elem : _line)
        if (_elem != ' ')
            return true;
    return false;
}

inline std::string trim(const std::ostringstream& _out) {
    std::string _trim;
    std::string _line;

    std::istringstream _input;
    _input.str(_out.str());

    while ( std::getline(_input, _line) )
        if (_line.size() != 0 && not_spaces(_line)) {
            _trim += _line;
            _trim += '\n'; }

    ltrim(_trim);
    rtrim(_trim);
    return _trim;
}

#endif /// _LIGHTWEIGHT_HPP