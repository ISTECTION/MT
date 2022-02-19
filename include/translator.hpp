#ifndef _TRANSLATOR_HPP
#define _TRANSLATOR_HPP
#include "const_table.hpp"
#include "var_table.hpp"
#include "error.hpp"
#include "token.hpp"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>               /// для функции isalpha


class translator
{
private:
    /// Постоянные таблицы
    const_table<std::string> operations;    /// 0
    const_table<std::string> keywords;      /// 1
    const_table<char> separators;           /// 2

    /// Переменные таблицы
    var_table identifiers;                  /// 3
    var_table constants;                    /// 4

    std::ostringstream nocomment_code;

    /// Директория для генерации файлов: tokens && error
    std::filesystem::path _directory;

public:
    explicit translator(const std::filesystem::path& _src_path)
        : keywords(Path_Const_Table::keywords),
          operations(Path_Const_Table::operations),
          separators(Path_Const_Table::separators),
          _directory(_src_path.parent_path()) {

        bool _is = analyse(_src_path);
        // if (_is == true)
        //     std::cout
        //         << "Generate files: "
        //         <<  _src_path.parent_path() / "token.txt"
        //         << '\n';
        // else
        //     std::cout
        //         << "Erorr file: "
        //         << _src_path.parent_path() / "error.txt"
        //         << '\n';
    }

private:
    bool analyse(const std::filesystem::path& );

};

bool translator::analyse(const std::filesystem::path& src_path) {

    std::ifstream fin_source(src_path);
    if (fin_source.is_open()) {
        std::ostringstream os_token, os_error;

        using _Iter_buf =
            std::istreambuf_iterator<char>;

        _Iter_buf eos, iit(fin_source);

        while (iit != eos) {

            /// Если символ является буквой
            if (std::isalpha(*iit)) {
                std::ostringstream _str;

                do {
                    nocomment_code << *iit;         /// Сохраняем код без комментариев
                    _str           << *iit;         /// Текущее слово

                    ++iit;
                    if (iit == eos)
                        return read_error_in_stream(
                                os_error, _ERROR::UNEXPECTED_SYMBOL);

                } while (std::isalpha(*iit));

                int _flag = keywords.get_num(_str.str());
                if (_flag != -1) {

                    // std::cout << _str.str() << " : " << _flag << '\n';
                    os_token << token(1, _flag, -1);

                } else {


                }
            }

            if (*iit == '/') {  /// Processing comments
                ++iit;

                if (iit == eos)
                    return read_error_in_stream(
                            os_error, _ERROR::UNEXPECTED_SYMBOL);

                if (*iit == '/')
                    do { ++iit; } while (*iit != '\n'); /// Возможно тут понадобится iit != oes
                else if (*iit == '*') {

                    char _preview,      /// Предыдущий символ
                         _current;      /// Текущий символ

                    do {
                        _preview = *(++iit);
                        if (iit == eos)
                            return read_error_in_stream(
                                    os_error, _ERROR::UNCLOSED_COMMENT);

                        if (*iit == '*') {
                            _current = *(++iit);

                            if (iit == eos)
                                return read_error_in_stream(
                                        os_error, _ERROR::UNCLOSED_COMMENT);
                        }
                    } while (_preview != '*' || _current != '/');
                } else
                     return read_error_in_stream(
                            os_error, _ERROR::UNEXPECTED_SYMBOL);
            } /// Processing comments

            ++iit;
        }
        std::cout << os_token.str() << std::endl;

        return true;

    } else {
        std::cerr
            << "file not open: "
            << src_path.string().c_str() << '\n';

        return false;
    }
}

#endif /// _TRANSLATOR_HPP