#ifndef _TRANSLATOR_HPP
#define _TRANSLATOR_HPP
#include "const_table.hpp"
#include "var_table.hpp"
#include "error.hpp"
#include "token.hpp"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <cassert>
#include <string>
#include <cctype>               /// Для функции isalpha и isalnum


using _Iter_buf = ::std::istreambuf_iterator<char>;


class translator
{
private:
    /// Постоянные таблицы
    const_table<std::string> operations;    /// 1
    const_table<std::string> keywords;      /// 2
    const_table<char> separators;           /// 3

    /// Переменные таблицы
    var_table identifiers;                  /// 4
    var_table constants;                    /// 5

    std::ostringstream nocomment_code;

    /// Директория для генерации файлов: tokens && error
    std::filesystem::path _directory;

public:
    explicit translator(const std::filesystem::path& _src_path)
        : operations(Path_Const_Table::operations),
          keywords(Path_Const_Table::keywords),
          separators(Path_Const_Table::separators),
          _directory(_src_path.parent_path()) {

        auto print_error =
            [](std::string_view filename) -> bool {
                std::cout
                    << "file not open: "
                    << filename << '\n';
                return false; };

        std::ifstream fin(_src_path);
        fin.is_open()
            ? analyse(fin)
            : assert(print_error(_src_path.stem().string()));
    }

private:
    void analyse(std::ifstream& );

    /// Очистка от комментариев
    std::optional<_ERROR> decomment(std::istreambuf_iterator<char>& );
};

std::optional<_ERROR>
translator::decomment (std::istreambuf_iterator<char>& iit) {
    using _eos_buf = ::std::istreambuf_iterator<char>;

    if (*iit != '/') return std::nullopt;
    if (++iit == _eos_buf())
        return std::optional { _ERROR::EOF_FILE };

    if (*iit == '/')
        do {
            if (++iit == _eos_buf())
                return std::nullopt;
        } while (*iit != '\n');
    else if (*iit == '*') {
        char _preview,      /// Предыдущий символ
             _current;      /// Текущий    символ
        do {
            _preview = *(++iit);
            if (iit == _eos_buf())
                return std::optional { _ERROR::UNCLOSED_COMMENT };

            if (*iit == '*') {
                _current = *(++iit);

                if (iit == _eos_buf())
                    return std::optional { _ERROR::UNCLOSED_COMMENT };
            }
        } while (_preview != '*' || _current != '/');
        /// После выхода из цикла итератор указывает на /
        ++iit;  /// Поэтому переходим на следующий символ
    } else
        return std::optional { _ERROR::UNEXPECTED_SYMBOL };

    return std::nullopt;
}

void translator::analyse (std::ifstream& _ifstream) {
    std::ostringstream os_token, os_error;

    _Iter_buf eos, iit(_ifstream);
    while (iit != eos) {

        std::optional<_ERROR> err_deccoment = decomment(iit);
        if (err_deccoment != std::nullopt)
            read_error_in_stream(os_error, err_deccoment.value());



        while (*iit == '\n' || *iit == '\t' || *iit == ' ') ++iit;


        std::cout << *iit;

        // ++iit;
    }
    std::cout << os_token.str() << std::endl;
    std::cout << os_error.str() << std::endl;

    // while (iit != eos) {

    //     std::cout << *iit;


    //     ++iit;
    // }


    //     while (iit != eos) {

    //         /// Если символ является буквой или '_'
    //         if (std::isalpha(*iit) || *iit == '_') {
    //             std::ostringstream _str;
    //             do {
    //                 nocomment_code << *iit;         /// Сохраняем код без комментариев
    //                 _str           << *iit;         /// Текущее слово

    //                 if (++iit == eos) return read_error_in_stream(
    //                         os_error, _ERROR::EOF_FILE);
    //             } while (std::isalnum(*iit) || *iit == '_');

    //             int _flag = keywords.get_num(_str.str());
    //             if (_flag != -1)
    //                 os_token
    //                     << token(TABLE::KEYWORDS, _flag, -1);
    //             else {
    //                 std::optional<place> pl =
    //                     identifiers.find_in_table(_str.str());

    //                 if (pl == std::nullopt)
    //                     pl = identifiers.add(_str.str());

    //                 using Pos = ::place::Pos;
    //                 place pl_value = pl.value();
    //                 os_token
    //                     <<  token(TABLE::IDENTIFIERS,
    //                         pl_value(Pos::ROW    ),
    //                         pl_value(Pos::COLLUMN));
    //             }
    //         } else if (separators.contains(*iit)) {
    //             nocomment_code << *iit;

    //             int flag = separators.get_num(*iit);
    //             if (flag != -1)
    //                 os_token << token(TABLE::SEPARATORS, flag, -1);

    //             ++iit;
    //         }
    //         else if (*iit == '/') {  /// Processing comments
    //             ++iit;

    //             if (iit == eos)
    //                 return read_error_in_stream(
    //                         os_error, _ERROR::UNEXPECTED_SYMBOL);

    //             if (*iit == '/')
    //                 do { ++iit; } while (*iit != '\n'); /// Возможно тут понадобится iit != oes
    //             else if (*iit == '*') {

    //                 char _preview,      /// Предыдущий символ
    //                      _current;      /// Текущий символ

    //                 do {
    //                     _preview = *(++iit);
    //                     if (iit == eos)
    //                         return read_error_in_stream(
    //                                 os_error, _ERROR::UNCLOSED_COMMENT);

    //                     if (*iit == '*') {
    //                         _current = *(++iit);

    //                         if (iit == eos)
    //                             return read_error_in_stream(
    //                                     os_error, _ERROR::UNCLOSED_COMMENT);
    //                     }
    //                 } while (_preview != '*' || _current != '/');
    //             } else
    //                  return read_error_in_stream(
    //                         os_error, _ERROR::UNEXPECTED_SYMBOL);
    //         } /// Processing comments

    //         while (*iit == '\n' || *iit == '\t' || *iit == ' ') ++iit;

    //         ++iit;
    //     }
    //     std::cout << os_token.str() << std::endl;
    //     std::cout << os_error.str() << std::endl;

    //     std::cout << identifiers;
    //     std::cout << keywords;


    //     return true;

    // } else {
    //     std::cerr
    //         << "file not open: "
    //         << src_path.string().c_str() << '\n';

    //     return false;
    // }
}

#endif /// _TRANSLATOR_HPP