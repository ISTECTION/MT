#ifndef _ERROR_HPP
#define _ERROR_HPP

enum class _ERROR {
    UNEXPECTED_SYMBOL,      ///< Неожиданный символ
    EOF_FILE,               ///< Неожиданный конец файла
    UNCLOSED_COMMENT        ///< Незакрытый комментарий
};


template <typename _Stream>
void read_error_in_stream (_Stream& _stream, _ERROR _error_code) {

    switch (_error_code) {
    case _ERROR::UNEXPECTED_SYMBOL:
        _stream << "unexpected symbol" << '\n';
        break;

    case _ERROR::EOF_FILE:
        _stream << "unexpected end file" << '\n';
        break;

    case _ERROR::UNCLOSED_COMMENT:
        _stream << "unclosed comment" << '\n';
        break;


    default:
        break;
    }
}

#endif /// _ERROR_HPP