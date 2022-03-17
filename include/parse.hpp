#ifndef _PARSE_HPP
#define _PARSE_HPP
#include "tabulate/tabulate.hpp"
#include "utils/lightweight.hpp"
#include "translator.hpp"
#include <filesystem>
#include <iostream>
#include <iterator>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <stack>

namespace parsing_table {
    std::filesystem::path
        parse_table = "file/const/parsing_table.txt";
}

class table_parse_elem {
public:
    std::vector<std::string> _terminal;         ///< Терминалы
    int  _jump;                                 ///< Переход
    bool _accept;                               ///< Принимать или нет
    bool _stack;                                ///< Класть в стек или нет
    bool _return;                               ///< Возвращать или нет
    bool _error;                                ///< Может ли быть ошибка
};

class parse
{
private:
    std::vector<table_parse_elem> table_parse;
    translator _trs;

    /// Количество ошибок
    size_t _count_error;


    /// Поток для записи ошибок
    std::ostringstream os_error;

public:
    explicit parse(const std::filesystem::path& _inp,
        std::optional<argparse::ArgumentParser> _prs = std::nullopt)
        : _trs(_inp, _prs),
          _count_error(0) {

        if (_trs.syntax_fail()) {
            std::cerr
                << "generate error file: "
                << (_inp.parent_path() / "error.txt").string()
                << '\n';
            assert(false);
        }

        std::ifstream fin(parsing_table::parse_table);
        fin.is_open()
            ? read_parse_table(fin)
            : assert(print_error(std::filesystem::canonical(parsing_table::parse_table).string()));
        fin.close();

        fin.open(_trs.get_parrent_path() / "token.txt");
        fin.is_open()
            ? base(fin)
            : assert(print_error(std::filesystem::canonical(_trs.get_parrent_path() / "token.txt").string()));
        fin.close();
    }

    friend std::ostream& operator<< (std::ostream& out, const parse& _prs);

    std::vector<table_parse_elem>::const_iterator begin () const { return table_parse.begin(); };
    std::vector<table_parse_elem>::const_iterator end   () const { return table_parse.end();   };


private:
    auto read_parse_table (std::ifstream& ) -> void;

    auto base (std::ifstream& ) -> void;
    auto LL_parse (std::ifstream& ) -> bool;
};

auto parse::read_parse_table (std::ifstream& fin) -> void {
    std::vector<std::string> words;
    auto record_vector =
        [&words](const std::string &word) -> void {
            words.push_back(word); };

    std::string _line;
    while (std::getline(fin, _line)) {
        std::istringstream istream(_line);
        std::for_each(std::istream_iterator<std::string>(istream),
            std::istream_iterator<std::string>(), record_vector );

        table_parse_elem parse_elem;
        size_t i = 0;
        for (; i < words.size() - 5; parse_elem._terminal.push_back(words.at(i++)));

        parse_elem._jump   = std::stoi(words.at(i++));
        parse_elem._accept = std::stoi(words.at(i++));
        parse_elem._stack  = std::stoi(words.at(i++));
        parse_elem._return = std::stoi(words.at(i++));
        parse_elem._error  = std::stoi(words.at(i++));

        table_parse.push_back(parse_elem);
        words.clear();
    }
}

auto parse::base (std::ifstream& fin) -> void {
    bool _error = LL_parse(fin);
}

auto parse::LL_parse (std::ifstream& fin_token) -> bool {
    using iterator_vec = std::vector<std::string>::const_iterator;

    token _token;                   ///< Токен из файла

    size_t current_row = 0;         ///< Текущая строка

    std::stack<size_t> _states;     ///< Стэк состояний


    fin_token >> _token;
    do {
        std::string token_text = _trs.get_token_text(_token);

        /// Итератор указывающий на ячейку элемента вектора _terminal
        iterator_vec _iter_str = find(table_parse[current_row]._terminal, token_text);

        /// Если итератор указывает на конец `ТЕРМИНАЛЬНОГО` вектора
        if (_iter_str == table_parse[current_row]._terminal.end()) {
            size_t _err = 0;
            /// Если это безальтернативная ошибка
            table_parse[current_row]._error
                /// Обрабатываем ошибку
                ? _err = stopper(os_error, SYNTACTIC::UNEXPECTED_TERMINAL, token_text)
                : current_row++;    /// Иначе выполняем поиск следующего варианта ветвления
            assert(not _err);       /// Если _err != 0 - ОШИБКА
        } else {


            /// Если нужно добавлять в стэк
            if (table_parse.at(current_row)._stack)
                _states.push(current_row + 1);

            ///
            if (table_parse.at(current_row)._accept) {




            }

            if (table_parse.at(current_row)._return) {

                if (_states.empty()) {
                    _count_error++;
                    return stopper(os_error, SYNTACTIC::STACK_IS_EMPTY, token_text);
                } else {
                    current_row = _states.top();
                    _states.pop();
                }

            } else
                current_row = table_parse.at(current_row)._jump;

        }

        /// Если елемент найден
        std::cout << token_text << ' ' << _token << '\n';


        // std::this_thread::sleep_for(std::chrono::milliseconds(200));

    } while(fin_token.good());

    return true;
}

std::ostream& operator<< (std::ostream& out, const parse& _prs) {
    auto bool_to_str = [](bool _init) -> std::string {
        return _init == true ? "1" : "0" ; };

    tabulate::Table movies;
    size_t id = 0;
    movies.add_row({ "id", "terminal", "jump", "accept", "stack", "return", "error" });
    for (auto iter = _prs.begin(); iter != _prs.end(); iter++) {
        std::ostringstream _terminal_line;
        for (auto it = iter->_terminal.begin(); it != iter->_terminal.end(); it++)
            _terminal_line << *it << ' ';

        movies.add_row({
            std::to_string(++id),
            _terminal_line.str(),
            std::to_string(iter->_jump),
            bool_to_str(iter->_accept),
            bool_to_str(iter->_stack),
            bool_to_str(iter->_return),
            bool_to_str(iter->_error)
        });
    }

    using namespace ::tabulate;
    movies.column(0).format().font_align(FontAlign::center).width(8);
    movies.column(1).format().font_align(FontAlign::center).width(32);
    movies.column(2).format().font_align(FontAlign::center).width(8);
    movies.column(3).format().font_align(FontAlign::center).width(8);
    movies.column(4).format().font_align(FontAlign::center).width(8);
    movies.column(5).format().font_align(FontAlign::center).width(8);
    movies.column(6).format().font_align(FontAlign::center).width(8);
    return out << movies << '\n';
}
#endif /// _PARSE_HPP