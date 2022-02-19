#ifndef _ERROR_HPP
#define _ERROR_HPP

enum class _ERROR {
    UNEXPECTED_SYMBOL,      /// Неожиданный символ
    UNCLOSED_COMMENT        /// Незакрытый комментарий
};


template <typename _Stream>
bool read_error_in_stream (_Stream& _stream, _ERROR _error_code) {

    switch (_error_code) {
    case _ERROR::UNEXPECTED_SYMBOL:
        _stream << "unexpected symbol" << '\n';
        break;

    case _ERROR::UNCLOSED_COMMENT:
        _stream << "unclosed comment" << '\n';
        break;


    default:
        break;
    }

    return false;
}

#endif /// _ERROR_HPP