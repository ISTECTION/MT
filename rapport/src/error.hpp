#ifndef _ERROR_HPP
#define _ERROR_HPP
#include "token.hpp"
#include <iomanip>

enum class SYNTACTIC : uint8_t {
    UNEXPECTED_TERMINAL = 1,
    UNDECLARED_TYPE,
    REPEAT_ANNOUNCEMENT,
    USE_UNINITIALIZED_VARIABLE,

    STACK_IS_EMPTY
};

template <typename _Stream>
auto stopper (_Stream& _stream, SYNTACTIC _ERR, std::size_t _current_line, const std::string& _terminal, std::vector<std::string> _maybe) -> size_t {

    std::string _LINE_ = '<' + std::to_string(_current_line) + '>';
    _stream << "syntax error" << std::setw(5) << std::left << _LINE_ << '|' << ' ';

    switch (_ERR) {
    case SYNTACTIC::UNEXPECTED_TERMINAL:
        _stream << "unexpected terminal: "          << _terminal << '\n'; break;
    case SYNTACTIC::STACK_IS_EMPTY:
        _stream << "stack is empty: "               << _terminal << '\n'; break;
    case SYNTACTIC::UNDECLARED_TYPE:
        _stream << "undeclared variable type: "     << _terminal << '\n'; break;
    case SYNTACTIC::REPEAT_ANNOUNCEMENT:
        _stream << "identifier alredy exists: "     << _terminal << '\n'; break;
    case SYNTACTIC::USE_UNINITIALIZED_VARIABLE:
        _stream << "using uninitialized variable: " << _terminal << '\n'; break;

    /// --------------- DEFAULT --------------- ///
    default: _stream << "error: " << _terminal << '\n';
    /// --------------- DEFAULT --------------- ///
    }

    _stream << "maybe you meant  |" << ' ';
    for (const auto& _term : _maybe) _stream << '"' << _term << '"' << ", ";

    return to_underlying(_ERR);
}
#endif /// _ERROR_HPP