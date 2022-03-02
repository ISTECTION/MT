#ifndef _TRANSLATOR_HPP
#define _TRANSLATOR_HPP
#include "utils/lightweight.hpp"
#include "utils/brackets.hpp"
#include "const_table.hpp"
#include "var_table.hpp"
#include "error.hpp"
#include "token.hpp"

#include <filesystem>
#include <iterator>
#include <sstream>
#include <fstream>
#include <cassert>
#include <string>
#include <cctype>               /// Для функции isalpha и isalnum
#include <stack>

using _Iter_buf = ::std::istreambuf_iterator<char>;

class translator
{
private:
    /// Стэк для хранения скобок
    std::stack<InfoBracket> _bracket;

    /// Постоянные таблицы
    const_table<std::string> operations;    /// 1
    const_table<std::string> keywords;      /// 2
    const_table<std::string> separators;    /// 3

    /// Переменные таблицы
    var_table identifiers;                  /// 4
    var_table constants;                    /// 5

    /// Поток для записи токенов и ошибок
    std::ostringstream os_token, os_error;

    /// Текущий номер линии
    std::string _current_str;
    size_t _current_lines;

    /// Поток для записи анализируемого кода без комментариев
    std::ostringstream nocomment_code;

    /// Директория для генерации файлов: tokens && error
    std::filesystem::path _directory;

public:
    explicit translator (const std::filesystem::path& _src_path)
        : operations(Path_Const_Table::operations),
          keywords(Path_Const_Table::keywords),
          separators(Path_Const_Table::separators),
          _directory(_src_path.parent_path()),
          _current_lines(1) {

        auto print_error =
            [](std::string_view filename) -> bool {
                std::cerr
                    << "file not open: "
                    << filename << '\n';
                return false; };

        std::ifstream fin(_src_path);
        fin.is_open()
            ? analyse(fin)
            : assert(print_error(_src_path.filename().string()));
    }

private:
    /// Лексический анализ
    void analyse(std::ifstream& );

    /// Очистка от комментариев
    std::optional<_ERROR> decomment (std::istreambuf_iterator<char>& );
    std::optional<_ERROR> lexical (std::istreambuf_iterator<char>& );

    /// РГЗ : Вариант 7
    std::optional<_ERROR> balanced (std::istreambuf_iterator<char>& );

    /// Пропустить ['\n', '\t', ' ']
    void skip_spaces (std::istreambuf_iterator<char>& );

};

std::optional<_ERROR>
translator::balanced (std::istreambuf_iterator<char>& iit) {

    if (open_brackets(*iit)) {
        _bracket.push(InfoBracket { _map_brackets[*iit], _current_lines });
        return std::nullopt;
    }

    if (close_brackets(*iit)) {
        if (_bracket.empty() ||
            _bracket.top().get_bracket() != *iit)
            return std::optional { _ERROR::BRACKET_MISTAKE };
        else _bracket.pop();
    }

    return std::nullopt;
}

void translator::skip_spaces (std::istreambuf_iterator<char>& iit) {
    for (; *iit == '\n' || *iit == '\t' || *iit == ' '; ++iit) {

        _current_str   += *iit;         /// Текущая строка
        nocomment_code << *iit;

        if (*iit == '\n') {
            _current_str.clear();
            _current_lines++; }
    }
}

std::optional<_ERROR>
translator::decomment (std::istreambuf_iterator<char>& iit) {
    using _eos_buf = ::std::istreambuf_iterator<char>;

    do {
        if (*iit != '/') return std::nullopt;
        if (++iit == _eos_buf())
            return std::optional { _ERROR::EOF_FILE };

        if (*iit == '/')
            do {
                if (++iit == _eos_buf())
                    return std::nullopt;
            } while (*iit != '\n');
        else if (*iit == '*') {
            char _preview,          /// Предыдущий символ
                 _current;          /// Текущий    символ

            if (++iit == _eos_buf())
                return std::optional { _ERROR::UNCLOSED_COMMENT };
            _current = *iit;

            do {
                /// Подсчёт строк в блочном комментарии
                if (*iit == '\n') _current_lines++;

                std::swap(_preview, _current);
                if (++iit == _eos_buf()) {
                    _current_str = "/*";
                    return std::optional { _ERROR::UNCLOSED_COMMENT }; }
                _current = *iit;
            } while (_preview != '*' || _current != '/');
            /// После выхода из цикла итератор указывает на /
            ++iit;  /// Поэтому переходим на следующий символ
            if (iit == _eos_buf())
                return std::nullopt;

        } else
            return std::optional { _ERROR::UNEXPECTED_SYMBOL };

        skip_spaces(iit);
    } while (*iit == '/');

    return std::nullopt;
}

std::optional<_ERROR>
translator::lexical (std::istreambuf_iterator<char>& iit) {
    using _eos_buf = ::std::istreambuf_iterator<char>;

    /// BEGIN: Если символ является буквой или '_'
    if (std::isalpha(*iit) || *iit == '_') {
        std::ostringstream _str;
        do {
            nocomment_code << *iit;         /// Сохраняем код без комментариев
            _str           << *iit;         /// Текущее слово
            _current_str   += *iit;         /// Текущая строка
            if (++iit == _eos_buf())
                return std::optional { _ERROR::EOF_FILE };

        } while (std::isalnum(*iit) || *iit == '_');

        int _flag = keywords.get_num(_str.str());
        if (_flag != -1) {
            os_token << token(TABLE::KEYWORDS, _flag, -1);
        }
        else {
            std::optional<place> _pl =
                    identifiers.find_in_table(_str.str());
            if (_pl == std::nullopt)
                _pl = identifiers.add(_str.str());

            place _pl_value = _pl.value();
            os_token << token(TABLE::IDENTIFIERS,
                    _pl_value(place::Pos::ROW),
                    _pl_value(place::Pos::COLLUMN));
        }
    }
    /// END:   Если символ является буквой или '_'
    /// BEGIN: Если символ является цифрой
    else if (std::isdigit(*iit)) {
        std::ostringstream _digit;
        do {
            nocomment_code << *iit;
            _digit         << *iit;
            _current_str   += *iit;
            if (++iit == _eos_buf())
                return std::optional { _ERROR::EOF_FILE };

        } while (std::isdigit(*iit));

        std::optional<place> _pl =
            constants.find_in_table(_digit.str());

        if (_pl == std::nullopt)
            _pl = constants.add(_digit.str());

        place _pl_value = _pl.value();
        os_token << token(TABLE::CONSTANTS,
                _pl_value(place::Pos::ROW),
                _pl_value(place::Pos::COLLUMN));
    }
    /// END: Если символ является цифрой
    /// BEGIN: Если символ является разделителем
    else if (separators.contains(std::string { *iit } )) {

        /// Обработка ошибок в употреблении скобок
        std::optional<_ERROR> err_bracket = balanced(iit);

        int flag = separators.get_num(std::string { *iit } );
        os_token << token(TABLE::SEPARATORS, flag, -1);
        nocomment_code << *iit;
        _current_str   += *iit;         /// Текущая строка

        ++iit;
        if (err_bracket != std::nullopt)
            return err_bracket;
    }
    /// END: Если символ является разделителем
    /// BEGIN: Если символ является операцией
    else if (operations.contains(std::string { *iit } )) {

        std::string _operation { *iit };
        nocomment_code << *iit;
        _current_str   += *iit;         /// Текущая строка
        if (++iit == _eos_buf())
            return std::optional { _ERROR::EOF_FILE };

        if (*iit == '+' || *iit == '-' ||
            *iit == '<' || *iit == '>') {

            nocomment_code << *iit;
            _operation     += *iit;
            _current_str   += *iit;         /// Текущая строка
            if (++iit == _eos_buf())
                return std::optional { _ERROR::EOF_FILE };
        }

        int flag = operations.get_num(_operation);
        if (flag == -1)
            return std::optional { _ERROR::OPERATION_NOT_EXIST };

        os_token << token(TABLE::OPERATION, flag, -1);
        return std::nullopt;
    }
    /// END: Если символ является операцией
    /// BEGIN: Если символ недопустим
    else {
        _current_str += *iit;         /// Текущая строка

        ++iit;
        return std::optional { _ERROR::UNEXPECTED_SYMBOL };
    }
    /// END: Если символ недопустим
    return std::nullopt;
}


void translator::analyse (std::ifstream& _ifstream) {
    _Iter_buf eos, iit(_ifstream);

    skip_spaces(iit);
    while (iit != eos) {
        std::optional<_ERROR> err_deccoment = decomment(iit);
        if (err_deccoment != std::nullopt)  {
            InfoError iErr {
                err_deccoment.value(),
                _current_lines,
                _current_str };
            os_error << iErr; }

        if (iit != eos) {
            std::optional<_ERROR> err_lexical = lexical(iit);
            if (err_lexical != std::nullopt) {
                InfoError iErr {
                    err_lexical.value(),
                    _current_lines,
                    _current_str };
                    os_error << iErr; }
        }

        skip_spaces(iit);
    }
    if (_bracket.size() != 0)
        os_error << InfoError {
            _ERROR::BRACKET_MISTAKE,
            _current_lines,
            std::string { _bracket.top().get_bracket() } };

    std::cout << trim(nocomment_code) << std::endl;
    std::cout << "token: \n" << os_token.str() << std::endl;
    std::cout << "error: \n" << os_error.str() << std::endl;

    std::cout << "keywords:    \n" << keywords;
    std::cout << "separators:  \n" << separators;
    std::cout << "identifiers: \n" << identifiers;
    std::cout << "constants:   \n" << constants;
    std::cout << "operations:  \n" << operations;
}
#endif /// _TRANSLATOR_HPP