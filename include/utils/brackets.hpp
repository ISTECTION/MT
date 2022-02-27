#ifndef _BRACKETS_HPP
#define _BRACKETS_HPP
#include <map>

class InfoBracket
{
private:
    char bracket;
    size_t line;

public:
    explicit InfoBracket(char _bracket, size_t _line)
        : bracket(_bracket),
          line(_line) { }

    char get_bracket () const { return bracket; }
    size_t get_line () const { return line; }
};

std::map<char, char> _map_brackets = {
        { '{', '}' },
        { '[', ']' },
        { '(', ')' },
        { '<', '>' } };

inline bool open_brackets (char c) {
    return
        c == '{' ||
        c == '[' ||
        c == '(' ||
        c == '<' ;}

inline bool close_brackets (char c) {
    return
        c == '}' ||
        c == ']' ||
        c == ')' ||
        c == '>' ;}

#endif /// _BRACKETS_HPP