#ifndef _ERROR_HPP
#define _ERROR_HPP
#include <iomanip>

enum class _ERROR {
    UNEXPECTED_SYMBOL,      ///< Неожиданный символ
    EOF_FILE,               ///< Неожиданный конец файла
    UNCLOSED_COMMENT,       ///< Незакрытый комментарий
    OPERATION_NOT_EXIST,    ///< Несуществующая операция


    BRACKET_MISTAKE         ///< Неравное количество закрывающихся-открывающихся скобок
};

class InfoError {
private:
    _ERROR error;       ///< Номер ошибка
    size_t line;        ///< Индекс строки

    std::string str;    ///< Строка

public:
    explicit InfoError (_ERROR _error, size_t _line, std::string _str)
      : error(_error),
        line (_line),
        str(_str) { }

    /**
     * @brief Возращаем enum с идентификатором ошибки
     *
     * @return _ERROR Номер ошибки
     */
    _ERROR get_error () const { return error; }

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
};

template <typename _Stream>
_Stream& operator<< (_Stream& _stream, const InfoError _info) {
    _stream << std::setw(4) << _info.get_line() << " | Error: ";

    switch (_info.get_error()) {
    case _ERROR::UNEXPECTED_SYMBOL:
        _stream << "unexpected symbol"      << '\n';    break;
    case _ERROR::EOF_FILE:
        _stream << "unexpected end file"    << '\n';    break;
    case _ERROR::UNCLOSED_COMMENT:
        _stream << "unclosed comment"       << '\n';    break;
    case _ERROR::OPERATION_NOT_EXIST:
        _stream << "non-existent operation" << '\n';    break;


    case _ERROR::BRACKET_MISTAKE:
        _stream << "incorrect use of brackets" << '\n'; break;

    /// --------------- DEFAULT --------------- ///
    default: _stream << "unidentified error" << '\n';
    /// --------------- DEFAULT --------------- ///
    }
    _stream << std::setw(4) << "" << " | " << _info.get_str() << '\n';
    return _stream;
}

#endif /// _ERROR_HPP