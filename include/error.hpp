#ifndef _ERROR_HPP
#define _ERROR_HPP
#include "token.hpp"
#include <iomanip>

/// ------- Лексические ошибки ------- ///
/// ENUM: Возможные лексические ошибки
enum class LEXICAL {
    UNEXPECTED_SYMBOL = 1,  ///< Неожиданный символ
    EOF_FILE,               ///< Неожиданный конец файла
    UNCLOSED_COMMENT,       ///< Незакрытый комментарий
    OPERATION_NOT_EXIST,    ///< Несуществующая операция


    BRACKET_MISTAKE,        ///< Неравное количество закрывающихся-открывающихся скобок
    DEFAULT                 ///< В конструктор передана строка с указанием ошибки
};

class InfoError {
private:
    LEXICAL error;       ///< Номер ошибка
    size_t line;        ///< Индекс строки

    std::string str;    ///< Строка
    std::string users_err;

public:
    explicit InfoError (LEXICAL _error, size_t _line, std::string _str)
      : error(_error),
        line (_line),
        str(_str) { }

    explicit InfoError (const std::string& _error, size_t _line, std::string _str)
      : users_err(_error),
        error(LEXICAL::DEFAULT),
        line(_line),
        str(_str) { }

    /**
     * @brief Возращаем enum с идентификатором ошибки
     *
     * @return LEXICAL Номер ошибки
     */
    LEXICAL get_error () const { return error; }

    /**
     * @brief Возвращает номер строки с ошибкой
     *
     * @return size_t Индекс строки
     */
    size_t get_line () const { return line; }

    /**
     * @brief Возвращает строку
     *
     * @return std::string Строка
     */
    std::string get_str () const { return str; }

    /**
     * @brief Возвращает строку с ошибкой
     *
     * @return std::string Строка
     */
    std::string get_users_err () const { return users_err; }
};

template <typename _Stream>
_Stream& operator<< (_Stream& _stream, const InfoError _info) {
    _stream << std::setw(4) << _info.get_line() << " | lexical error: ";

    switch (_info.get_error()) {
    case LEXICAL::UNEXPECTED_SYMBOL:
        _stream << "unexpected symbol"      << '\n';    break;
    case LEXICAL::EOF_FILE:
        _stream << "unexpected end file"    << '\n';    break;
    case LEXICAL::UNCLOSED_COMMENT:
        _stream << "unclosed comment"       << '\n';    break;
    case LEXICAL::OPERATION_NOT_EXIST:
        _stream << "non-existent operation" << '\n';    break;


    case LEXICAL::BRACKET_MISTAKE:
        _stream << "incorrect use of brackets" << '\n'; break;

    /// --------------- DEFAULT --------------- ///
    default: _stream << _info.get_users_err() << '\n';
    /// --------------- DEFAULT --------------- ///
    }
    _stream << std::setw(4) << "" << " | " << _info.get_str() << '\n';
    return _stream;
}
/// ------- Лексические ошибки ------- ///


/// ----- Синтаксические ошибки ----- ///
/// ENUM: Возможные синтаксические ошибки
enum class SYNTACTIC {
    UNEXPECTED_TERMINAL = 1,    ///< Неожиданный терминал
    UNDECLARED_TYPE,            ///< Необъявленный тип переменной
    REPEAT_ANNOUNCEMENT,        ///< Повторное объявление переменной
    USE_UNINITIALIZED_VARIABLE, ///< Употребление неинициализированной переменной в выражении

    STACK_IS_EMPTY              ///< Пустой `_states` стэк
};

/// Заглушка (Возвращает номер ошибки)
template <typename _Stream>
auto stopper (_Stream& _stream, SYNTACTIC _ERR, const std::string& _terminal, std::vector<std::string> _maybe) -> size_t {
    _stream << "syntax error" << std::setw(5) << '|' << ' ';

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

    _stream << "maybe you meant" << std::setw(2) << '|' << ' ';
    for (const auto& _term : _maybe) _stream << '"' << _term << '"' << ", ";

    return to_underlying(_ERR);
}
/// ----- Синтаксические ошибки ----- ///

#endif /// _ERROR_HPP