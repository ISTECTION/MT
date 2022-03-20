#ifndef _PARSE_HPP
#define _PARSE_HPP
#include "stringswitch/hash.hpp"
#include "tabulate/tabulate.hpp"
#include "utils/lightweight.hpp"
#include "argparse/argparse.hpp"
#include "translator.hpp"

#include <string_view>
#include <filesystem>       /// std::filesystem::path
#include <iostream>         /// std::cout std::cerr
#include <iterator>         /// std::vector<T>::iterator
#include <cassert>          /// assert
#include <fstream>          /// std::ifstream
#include <sstream>          /// std::ostringstream
#include <vector>           /// std::vector
#include <thread>           ///
#include <chrono>           /// std::chrono
#include <string>           /// std::string
#include <queue>            /// std::queue - очередь    (постфиксная запись)
#include <stack>            /// std::stack - стэк       (постфиксная запись)

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

class parse : public translator
{
private:
    std::vector<table_parse_elem> table_parse;  ///< Постоянная таблица для переходов

    size_t _count_error;            ///< Количество ошибок

    std::ostringstream os_error;    ///< Поток для записи ошибок

    std::ostringstream os_postfix;  ///< Поток для записи постфиксной записи выражений

public:
    explicit parse(const std::filesystem::path& _inp)
        : translator(_inp),
          _count_error(0) {

        if (this->syntax_fail()) {
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

        fin.open(this->get_parrent_path() / "token.txt");
        fin.is_open()
            ? base(fin)
            : assert(print_error(std::filesystem::canonical(this->get_parrent_path() / "token.txt").string()));
        fin.close();
    }

    /**
     * @brief Перегрузка оператора `operator<<` для вывода постоянно таблицы в поток
     *
     * @param out  Поток (Например: std::cout, std::ofstream)
     * @param _prs Екземпляр класса parse
     * @return std::ostream&
     */
    friend std::ostream& operator<< (std::ostream& out, const parse& _prs);

    /**
     * @brief Функция возвращает константный указатель на начало постоянной таблицы синтаксического анализатора
     *
     * @return std::vector<table_parse_elem>::const_iterator
     */
    std::vector<table_parse_elem>::const_iterator begin () const { return table_parse.begin(); };

    /**
     * @brief Функция возвращает константный указатель на конец постоянной таблицы синтаксического анализатора
     *
     * @return std::vector<table_parse_elem>::const_iterator
     */
    std::vector<table_parse_elem>::const_iterator end () const { return table_parse.end(); };

private:

    /**
     * @brief Функция читает постоянную таблицу `file/const/parsing_table.txt` в вектор структур 'table_parse'
     *
     */
    auto read_parse_table (std::ifstream& ) -> void;

    /**
     * @brief Базовая функция синтаксического анализатора с которой начинается запуск процесса
     *
     */
    auto base (std::ifstream& ) -> void;

    /**
     * @brief LL-анализатор (LL-parser)
     *
     * @return true  Если не было выявлено синтаксических ошибок
     * @return false Если была замечена ошибка
     */
    auto LL_parse (std::ifstream& ) -> bool;

    /**
     * @brief Функция преобразует инфиксную форму записи выражения в постфиксную
     *
     */
    auto make_postfix (const std::vector<token>& ) -> void;

    /**
     * @brief Функция сравнивает приоритеты операций
     *
     * @param _left  Левая операция  (Находящаяся на вершине стэка при построении постфиксной формы)
     * @param _right Правая операция (Текущая)
     * @return true  Если приоритет правой оперции >= левой
     * @return false Если приоритет правой оперции <  левой
     */
    auto priority (const std::string& _left, const std::string& _right) -> bool;
};

auto parse::read_parse_table (std::ifstream& fin) -> void {
    std::vector<std::string> words;
    auto record_vector =
        [&words](const std::string &word) -> void {
            words.push_back(word); };

    table_parse.push_back(table_parse_elem {
        { "void", "int" },  1,
        false, true, false, true });

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
    if (_error == false) {
        std::cerr << "lexical analyzer has detected error" << '\n';
    }
    std::ofstream fout(this->get_parrent_path() / "postfix.txt");
    fout << os_postfix.str();
    fout.close();

    fout.open(this->get_parrent_path() / "syntactic_error.txt");
    fout << os_error.str();
    fout.close();
}

auto parse::LL_parse (std::ifstream& fin_token) -> bool {
    using iterator_vec = std::vector<std::string>::const_iterator;

    bool _postfix = false;                  ///< Нужно ли выполнять построение постфиксной записи для данной строки
    size_t current_row = 0;                 ///< Текущая строка таблицы parse_table

    token _token;                           ///< Исследуемый токен из файла
    TYPE is_set_type = TYPE::UNDEFINED;     ///< Тип идентификатора (TYPE::UNDEFINED - если не задан)

    std::stack<size_t> _states;             ///< Стэк состояний (В нём хранятся индексы строк для перехода, после встречи _jump == -1)
    std::vector<token> _infix_token_arr;    ///< Вектор токенов выражения в инфиксной форме

    fin_token >> _token;                    /// Записываем первый токен
    do {
        std::string token_text = this->get_token_text(_token);
        /// Итератор указывающий на ячейку элемента вектора _terminal
        iterator_vec _iter_str = find(table_parse[current_row]._terminal, token_text);

        /// Если итератор указывает на конец `ТЕРМИНАЛЬНОГО` вектора
        if (_iter_str == table_parse[current_row]._terminal.end()) {
            size_t _err = 0;    /// Ошибка
            /// Если это безальтернативная ошибка
            table_parse[current_row]._error
                /// Обрабатываем ошибку
                ? _err = stopper(os_error, SYNTACTIC::UNEXPECTED_TERMINAL, token_text)
                : current_row++;    /// Иначе выполняем поиск следующего варианта ветвления
            if (_err != 0) break;

        } else {
            /// Если нужно добавлять в стэк
            if (table_parse.at(current_row)._stack)
                _states.push(current_row + 1);

            /// Если это принимаемый токен (В конце разбора берем следующий токен)
            if (table_parse.at(current_row)._accept) {

                /// Если `=`, то будет EXPR, включаем постфиксную запись
                if (token_text == "var") { _postfix = true; }

                if (_postfix == true) {
                    /// Если это унарный минус
                    if (current_row == 50) {
                        std::optional<place> _pl = this->constants.contains("-1")
                            ? this->constants.find_in_table("-1")
                            : this->constants.add("-1") ;

                        using enum ::place::POS;
                        std::size_t _row = _pl.value()(ROW);
                        int _col = static_cast<int>(_pl.value()(COLLUMN));

                        /// Добавляем -1 в инфиксную запись
                        _infix_token_arr.push_back(token { TABLE::CONSTANTS, _row, _col });

                        /// Добавляем знак умножения в инфиксную запись
                        std::size_t _position = static_cast<std::size_t>(this->operations.get_num("*"));
                        _infix_token_arr.push_back(token { TABLE::OPERATION, _position, -1 });
                    }
                    /// Иначе добавляем токен в инфиксную запись
                    else { _infix_token_arr.push_back(_token); }
                }

                /// Если закончили разбор присваивания или части объявления
                if (token_text == "," || token_text == ";") {
                    /// Добавим все, что разобрали в постфиксную запись
                    /// 2 токена будет в случае обычного объявления переменной
                    /// Например: int a; | 1-ый токен: a | 2-ой токен: ; |
                    /// В этом случае не будем строить постфиксную запись выражения
                    if (_infix_token_arr.size() > 2) { make_postfix(_infix_token_arr); }
                    _infix_token_arr.clear();           /// Очищаем вектор
                    _postfix = false;                   /// Выключаем постфиксную запись
                }

                /// Если закончили разбор строки, сбрасываем флаг объявления
                if (token_text == ";") is_set_type = TYPE::UNDEFINED;

                /// Если попался тип, запоминаем его
                using namespace _switch::literals;
                switch (_switch::hash(token_text))
                {
                    case  "int"_hash: is_set_type = TYPE::INT;  break;
                    case "char"_hash: is_set_type = TYPE::CHAR; break;
                }

                if (token_text == "var" && is_set_type != TYPE::UNDEFINED && current_row == 69) {
                    std::optional<lexeme> _lexeme = this->identifiers.get_lexeme(_token.get_place());
                    if (_lexeme.value().get_type() != TYPE::UNDEFINED) {
                        _count_error++;
                        return stopper(os_error, SYNTACTIC::REPEAT_ANNOUNCEMENT, _lexeme.value().get_name());
                    }
                    this->identifiers.set_type(_token.get_place(), is_set_type);
                }

                /// Обработка необъявленного типа идентификатора
                if (token_text == "var" && (current_row == 46 || current_row == 97)) {
                    std::optional<lexeme> _lexeme = this->identifiers.get_lexeme(_token.get_place());

                    if (_lexeme.value().get_type() == TYPE::UNDEFINED) {
                        _count_error++;
                        return stopper(os_error, SYNTACTIC::UNDECLARED_TYPE, _lexeme.value().get_name());
                    }
                }

                fin_token >> _token;
            }
            /// Если поле _jump == -1, то перемещаемся на строку записанную в вершине стэка
            if (table_parse.at(current_row)._return) {

                if (_states.empty()) {
                    _count_error++;
                    return stopper(os_error, SYNTACTIC::STACK_IS_EMPTY, token_text);
                } else {
                    current_row = _states.top();
                    _states.pop();
                }
            /// Если поле _return != 1, то перемешаемся на строку _jump таблицы table_parse
            } else { current_row = table_parse.at(current_row)._jump; }

        }

    /// Пока не конец файла
    } while(fin_token.good());

    return true;
}

auto parse::make_postfix (const std::vector<token>& _infix_token_arr) -> void {
    std::queue<std::string> _queue_postfix;
    std::stack<std::string> _stack_postfix;

    for (std::size_t i = 0; i < _infix_token_arr.size(); i++) {
        std::string token_text = this->get_token_text(_infix_token_arr[i]);

        TABLE t_table = _infix_token_arr[i].get_table();

        /// Если это идентификатор или константа
        if (t_table == TABLE::IDENTIFIERS || t_table == TABLE::CONSTANTS) {
            place _pl = _infix_token_arr[i].get_place();
            _queue_postfix.push(this->get_var_table(t_table).get_lexeme(_pl).value().get_name());
        }
        /// Если это операция
        else if (t_table == TABLE::OPERATION) {
            while (
                _stack_postfix.size() > 0                       &&  /// Пока в стэке присутствуют элементы
                this->operations.contains(_stack_postfix.top()) &&  /// Если это операция
                priority(_stack_postfix.top(), token_text)) {       /// Если token_text имеет более низкий приоритет чем _stack_postfix.top()

                _queue_postfix.push(_stack_postfix.top());
                _stack_postfix.pop();
            }
            _stack_postfix.push(token_text);
        }
        /// Если это открывающая скобка
        else if (token_text == "(") {
            _stack_postfix.push(token_text);
        }
        /// Если это закрывающая скобка
        else if (token_text == ")") {

            while (_stack_postfix.top() != "(") {
                _queue_postfix.push(_stack_postfix.top());
                _stack_postfix.pop();
            }
            /// Снимаем с вершины `(`
            _stack_postfix.pop();
        }

    }

    /// Выгружаем стэк в очередь
    while (not _stack_postfix.empty()) {
        _queue_postfix.push(_stack_postfix.top());
        _stack_postfix.pop();
    }

    /// Последним токен в инфиксном векторе будет разделитель `;` или `,`
    std::string back_token_text = this->get_token_text(_infix_token_arr.back());
    _queue_postfix.push(back_token_text);

    /// Перенос постфиксной очереди в поток
    while (not _queue_postfix.empty()) {
        os_postfix << _queue_postfix.front() << ' ';
        _queue_postfix.pop();
    }
}

auto parse::priority (const std::string& _left, const std::string& _right) -> bool {
    std::size_t _left_priority = this->operations.get_priority(_left);
    std::size_t _right_priority = this->operations.get_priority(_right);
    return _right >= _left;
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
            std::to_string(id++),
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