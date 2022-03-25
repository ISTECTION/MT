#ifndef _TRANSLATOR_HPP
#define _TRANSLATOR_HPP
#include "utils/lightweight.hpp"
#include "utils/brackets.hpp"
#include "const_table.hpp"
#include "var_table.hpp"
#include "toml++/toml.h"
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

    /// Поток для записи токенов и ошибок
    std::ostringstream os_token, os_error;
    size_t _count_error;

    /// Текущий номер линии
    std::string _current_str;
    size_t      _current_lines;

    /// Поток для записи анализируемого кода без комментариев
    std::ostringstream nocomment_code;

    /// Директория для генерации файлов: tokens && error
    std::filesystem::path _parent_path;

    /// Массив токенов текущей строки, для записи TOML файла
    toml::array toml_array;

public:
    explicit translator (const std::filesystem::path& _src_path)
        : operations(path_const_table::operations),
          keywords(path_const_table::keywords),
          separators(path_const_table::separators),
          _parent_path(_src_path.parent_path()),
          _current_lines(1),
          _count_error(0) {

        auto print_error =
            [](std::string_view filename) -> bool {
                std::cerr
                    << "file not open: "
                    << filename << '\n';
                return false; };

        std::ifstream fin(_src_path);
        fin.is_open()
            ? analyse(fin)
            : assert(print_error(std::filesystem::canonical(_src_path.filename()).string()));
        fin.close();
    }

    /**
     * @brief Функция возвращает true, если не было обнаружено ошибок при лексическом анализе
     *
     * @return true  Успех
     * @return false Неудача
     */
    bool syntax_success () const { return _count_error ? false : true; }

    /**
     * @brief Функция возвращает true, если была обнаружена хотя бы одна ошибка при лексическом анализе
     *
     * @return true  Одна и более ошибок
     * @return false Лексический анализатор не обнаружил ошибок
     */
    bool syntax_fail    () const { return not syntax_success(); }

    /**
     * @brief Возвращает родительскую дирректорию файла, который был отправлен на анализ
     *
     * @return std::filesystem::path Путь
     */
    std::filesystem::path get_parrent_path () const { return _parent_path; }

    /**
     * @brief Возвращает строкове значение имени токена
     *
     * @param _tkn Токен в виде (TABLE, I, J)
     * @return std::string Строкове значение константной таблицы или var - в случае индентификатора и const - константы
     */
    std::string get_token_text (const token& _tkn) const;

    /**
     * @brief Возвращает ссылку на одну из переменных таблицу
     *
     * @param _table Номер таблицы (TABLE::IDENTIFIERS или TABLE::CONSTANTS)
     * @return var_table& Ссылка
     */
    var_table& get_var_table(TABLE _table) {

        if (_table == TABLE::IDENTIFIERS || _table == TABLE::CONSTANTS) {
            return _table == TABLE::IDENTIFIERS
                ? identifiers
                : constants ;
        } else
            throw std::runtime_error("not table: (TABLE::IDENTIFIERS || TABLE::CONSTANTS)");
    }

    /**
     * @brief Распечать таблицу
     *
     * @param _prs_table true - на консоль и в файл, false - только в файл
     */
    void print_table (bool _prs_table) const;

private:

    /**
     * @brief Лексический анализатор
     *
     */
    void analyse(std::ifstream& );

    /**
     * @brief Функция очистки кода от комментариев (от текущего положения и до конца комментария)
     *
     * @return std::optional<LEXICAL> Ошибка. Например: открытый комментарий в случае конца файла
     */
    std::optional<LEXICAL> decomment (std::istreambuf_iterator<char>& );

    /**
     * @brief Обработка всех возможных цепочек языка, таких как идентификаторы, константы, ключевые слова, операции и разделители
     *
     * @return std::optional<LEXICAL> Ошибка. Например: внезапная буква в цепочке цифр `int size = 123a;`
     */
    std::optional<LEXICAL> lexical (std::istreambuf_iterator<char>& );

    /**
     * @brief РГЗ : Вариант 7 (Ошибки в употреблении скобок)
     *
     * @return std::optional<LEXICAL> Ошибка в случае неравного количеста открывающих и закрывающих скобок
     */
    std::optional<LEXICAL> balanced (std::istreambuf_iterator<char>& );

    ///

    /**
     * @brief Функция пропускает все виды пробелов, такие как - ['\n', '\t', ' ']
     *
     */
    void skip_spaces (std::istreambuf_iterator<char>& );

protected:
    const_table_operation<std::string> operations;  ///< Постоянная таблица #1 (Таблица операций и приоритетов)
    const_table<std::string> keywords;              ///< Постоянная таблица #2 (Таблица ключевых слов)
    const_table<std::string> separators;            ///< Постоянная таблица #3 (Таблица разделителей)

    var_table identifiers;                          ///< Переменная таблица #4 (Таблица идентификаторов)
    var_table constants;                            ///< Переменная таблица #5 (Таблица констант)
};

std::string
translator::get_token_text (const token& _tkn) const {
    switch (_tkn.get_table())
    {
        case TABLE::OPERATION  : return operations.get_elem(_tkn.get_row()).value(); break;
        case TABLE::KEYWORDS   : return keywords  .get_elem(_tkn.get_row()).value(); break;
        case TABLE::SEPARATORS : return separators.get_elem(_tkn.get_row()).value(); break;
        // case TABLE::IDENTIFIERS: return identifiers.get_lexeme(place(_tkn.get_row(), _tkn.get_column())).value().get_name(); break;
        // case TABLE::CONSTANTS  : return constants  .get_lexeme(place(_tkn.get_row(), _tkn.get_column())).value().get_name(); break;
        case TABLE::IDENTIFIERS: return std::string {  "var"  }; break;
        case TABLE::CONSTANTS  : return std::string { "const" }; break;

        default: return std::string { };
    }
}


std::optional<LEXICAL>
translator::balanced (std::istreambuf_iterator<char>& iit) {
    if (open_brackets(*iit)) {
        _bracket.push(
            InfoBracket { _map_brackets[*iit], _current_lines });
        return std::nullopt;
    } else if (close_brackets(*iit)) {
        if (_bracket.empty() ||
            _bracket.top().get_bracket() != *iit)
            return std::optional { LEXICAL::BRACKET_MISTAKE };
        else _bracket.pop();
    }
    return std::nullopt;
}


void translator::skip_spaces (std::istreambuf_iterator<char>& iit) {
    using _eos_buf = ::std::istreambuf_iterator<char>;

    if (iit == _eos_buf()) return;
    for ( ; *iit == '\n' || *iit == '\t' || *iit == ' '; ++iit) {
        _current_str   += *iit;         /// Текущая строка
        nocomment_code << *iit;

        if (*iit == '\n') {
            _current_str.clear();
            _current_lines++;
        }
    }
}

std::optional<LEXICAL>
translator::decomment (std::istreambuf_iterator<char>& iit) {
    using _eos_buf = ::std::istreambuf_iterator<char>;

    do {
        if (*iit != '/') return std::nullopt;
        if (++iit == _eos_buf())
            return std::optional { LEXICAL::EOF_FILE };

        if (*iit == '/')
            do {
                if (++iit == _eos_buf())
                    return std::nullopt;
            } while (*iit != '\n');
        else if (*iit == '*') {
            char _preview,          /// Предыдущий символ
                 _current;          /// Текущий    символ

            if (++iit == _eos_buf())
                return std::optional { LEXICAL::UNCLOSED_COMMENT };
            _current = *iit;

            do {
                /// Подсчёт строк в блочном комментарии
                if (*iit == '\n') _current_lines++;

                std::swap(_preview, _current);
                if (++iit == _eos_buf()) {
                    _current_str = "/*";
                    return std::optional { LEXICAL::UNCLOSED_COMMENT }; }
                _current = *iit;
            } while (_preview != '*' || _current != '/');
            /// После выхода из цикла итератор указывает на /
            ++iit;  /// Поэтому переходим на следующий символ
            if (iit == _eos_buf())
                return std::nullopt;

        } else
            return std::optional { LEXICAL::UNEXPECTED_SYMBOL };

        if (toml_array.size() > 0) {
            os_token << std::setw(4) << std::setfill('0')
                << _current_lines << " = " << toml_array << '\n';
            toml_array.clear();
        }

        skip_spaces(iit);
    } while (*iit == '/');

    return std::nullopt;
}

std::optional<LEXICAL>
translator::lexical (std::istreambuf_iterator<char>& iit) {
    using _eos_buf = ::std::istreambuf_iterator<char>;
    std::ostringstream _token_text; ///< Токен в виде (_, _, _)

    /// BEGIN: Если символ является буквой или '_'
    if (std::isalpha(*iit) || *iit == '_') {
        std::ostringstream _str;
        do {
            nocomment_code << *iit;         /// Сохраняем код без комментариев
            _str           << *iit;         /// Текущее слово
            _current_str   += *iit;         /// Текущая строка
            if (++iit == _eos_buf())
                return std::optional { LEXICAL::EOF_FILE };

        } while (std::isalnum(*iit) || *iit == '_');

        int _flag = keywords.get_num(_str.str());
        if (_flag != -1) {
            _token_text << token(TABLE::KEYWORDS, _flag, -1);
            toml_array.insert(toml_array.end(), _token_text.str());
        }
        else {
            std::optional<place> _pl = identifiers.find_in_table(_str.str());
            if (_pl == std::nullopt)
                _pl = identifiers.add(_str.str());

            using enum ::place::POS;
            place _pl_value = _pl.value();

            _token_text << token(TABLE::IDENTIFIERS, _pl_value(ROW), _pl_value(COLLUMN));
            toml_array.insert(toml_array.end(), _token_text.str());

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
                return std::optional { LEXICAL::EOF_FILE };

        } while (std::isdigit(*iit));

        if (std::isalpha(*iit) || *iit == '_') {
            _current_str   += *iit;
            return std::optional { LEXICAL::UNEXPECTED_SYMBOL };
        }

        std::optional<place> _pl =
            constants.find_in_table(_digit.str());

        if (_pl == std::nullopt)
            _pl = constants.add(_digit.str());

        using enum ::place::POS;
        place _pl_value = _pl.value();

        _token_text << token(TABLE::CONSTANTS, _pl_value(ROW), _pl_value(COLLUMN));
        toml_array.insert(toml_array.end(), _token_text.str());
    }
    /// END: Если символ является цифрой
    /// BEGIN: Если символ является разделителем
    else if (separators.contains(std::string { *iit } )) {

        /// Обработка ошибок в употреблении скобок
        std::optional<LEXICAL> err_bracket = balanced(iit);

        int flag = separators.get_num(std::string { *iit } );

        _token_text << token(TABLE::SEPARATORS, flag, -1);
        toml_array.insert(toml_array.end(), _token_text.str());
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
            return std::optional { LEXICAL::EOF_FILE };

        if (*iit == '+' || *iit == '-' ||
            *iit == '<' || *iit == '>') {

            nocomment_code << *iit;
            _operation     += *iit;
            _current_str   += *iit;         /// Текущая строка
            if (++iit == _eos_buf())
                return std::optional { LEXICAL::EOF_FILE };
        }

        int flag = operations.get_num(_operation);
        if (flag == -1)
            return std::optional { LEXICAL::OPERATION_NOT_EXIST };

        _token_text << token(TABLE::OPERATION, flag, -1);
        toml_array.insert(toml_array.end(), _token_text.str());

        return std::nullopt;
    }
    /// END: Если символ является операцией
    /// BEGIN: Если символ недопустим
    else {
        _current_str += *iit;         /// Текущая строка

        ++iit;
        return std::optional { LEXICAL::UNEXPECTED_SYMBOL };
    }
    /// END: Если символ недопустим
    return std::nullopt;
}

void translator::analyse (std::ifstream& _ifstream) {
    _Iter_buf eos, iit(_ifstream);


    skip_spaces(iit);
    while (iit != eos) {
        std::optional<LEXICAL> err_deccoment = decomment(iit);
        if (err_deccoment != std::nullopt)  {
            InfoError iErr { err_deccoment.value(), _current_lines, _current_str };

            os_error << iErr;
            _count_error++;
        }

        if (iit != eos) {
            std::optional<LEXICAL> err_lexical = lexical(iit);
            if (err_lexical != std::nullopt) {
                InfoError iErr { err_lexical.value(), _current_lines, _current_str };

                os_error << iErr;
                _count_error++;
            }
        }

        if (iit != eos && *iit == '\n') {
            os_token << std::setw(4) << std::setfill('0')
                << _current_lines << " = " << toml_array << '\n';
            toml_array.clear();
        }

        skip_spaces(iit);
    }

    if (toml_array.size() > 0) {
        os_token << std::setw(4) << std::setfill('0') << _current_lines << " = " << toml_array; }

    if (_bracket.size() != 0) {
        os_error << InfoError {
            LEXICAL::BRACKET_MISTAKE,
            _current_lines,
            "lack: " + std::string { _bracket.top().get_bracket() } };
        _count_error++;
    }

    std::ofstream fout(_parent_path / "token.toml");
    fout << os_token.str();
    fout.close();

    fout.open(_parent_path / "lexical_error.txt");
    fout << os_error.str();
    fout.close();

    fout.open(_parent_path / "clearcode.cpp");
    fout << trim(nocomment_code);
    fout.close();
}

void translator::print_table (bool _prs_table) const {

    std::ofstream fout(_parent_path / "table.txt");
    fout << "keywords:    \n" << keywords;
    fout << "separators:  \n" << separators;
    fout << "identifiers: \n" << identifiers;
    fout << "constants:   \n" << constants;
    fout << "operations:  \n" << operations;
    fout.close();

    if (_prs_table) {
        std::cout << "keywords:    \n" << keywords;
        std::cout << "separators:  \n" << separators;
        std::cout << "identifiers: \n" << identifiers;
        std::cout << "constants:   \n" << constants;
        std::cout << "operations:  \n" << operations;
    }
}

#endif /// _TRANSLATOR_HPP