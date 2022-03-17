#ifndef _VAR_TABLE_HPP
#define _VAR_TABLE_HPP
#include "tabulate/tabulate.hpp"
#include "lexeme.hpp"
#include "place.hpp"

#include <functional>
#include <optional>
#include <vector>
#include <set>

/**
 * @brief Класс хранящий лексемы
 */
class var_table
{
private:
    std::vector<std::vector<lexeme>> table;

public:
    explicit var_table(size_t _size = 26 * 2 + 1) {
        table.resize(_size); }

    /**
     * @brief Проверяет существования идентификатора в таблице
     *
     * @param name   Строка для поиска
     * @return true  Если такой идентификатор найден
     * @return false Если такой идентификатор не найден
     */
    bool contains (const std::string& name) const;

    /**
     * @brief Функция добавляет идентификатор в таблицу
     *
     * @param name Имя идентификатора
     * @return std::optional<place> Будет иметь значение std::nullopt, если элемент уже существует
     */
    std::optional<place> add (const std::string& name);
    /**
     * @brief Поиск идентификатора в таблице
     *
     * @param name Имя идентификатора
     * @return std::optional<place> Будет иметь значение std::nullopt, если элемент не найден
     */
    std::optional<place> find_in_table (const std::string& name) const;

    /**
     * @brief Устанавить тип объекту
     *
     * @param plc  Класс хранящий позицию лексемы
     * @param type Значение типа
     */
    var_table& set_type  (const place& plc, TYPE type );

    /**
     * @brief Устанавить значение объекту
     *
     * @param plc   Класс хранящий позицию лексемы
     * @param value Принимает true, если значение было инициализировано
     */
    var_table& set_value (const place& plc, bool value);

    /**
     * @brief Получить лексему
     *
     * @param _place Позиция лексемы в таблице
     * @return std::optional<lexeme> Будет иметь значение std::nullopt, если указан неверная позиция
     */
    std::optional<lexeme> get_lexeme (const place& _place) const;

    /**
     * @brief Возвращает константный итератор на начало таблицы
     */
    std::vector<std::vector<lexeme>>::const_iterator begin () const;

    /**
     * @brief Возвращает константный итератор на конец таблицы
     */
    std::vector<std::vector<lexeme>>::const_iterator end () const;

    /**
     * @brief Возвращает хэш входной строки
     *
     * @param name    Строка, от которой нужно вычислить хэш
     * @return size_t Остаток от деления Хэша на Размер таблицы
     */
    inline size_t get_hash (const std::string& name) const;

    /**
     * @brief Получить размерность таблицы
     *
     * @return size_t Размерность таблицы
     */
    inline size_t get_size_table () const { return table.size(); }

    /**
     * @brief Получить размерность строки таблицы
     *
     * @param row  Индекс строки
     * @return int Размерность таблицы
     */
    inline int get_size_row (size_t row) const {
        return row < table.size()
            ? table[row].size()
            : -1 ; }

    /**
     * @brief Выводит таблицу в поток
     */
    friend std::ostream& operator<< (std::ostream& out, const var_table& _tab);
};

bool var_table::contains (const std::string& name) const {
    return find_in_table(name) == std::nullopt
        ? false : true;
}

std::optional<lexeme> var_table::get_lexeme (const place& _place) const {
    using enum ::place::POS;
    size_t i = _place(ROW);
    size_t j = _place(COLLUMN);
    return static_cast<int>(j) < get_size_row(i)
        ? std::optional { table[i][j] }
        : std::nullopt ;
}

std::optional<place> var_table::add (const std::string& name) {
    if (contains(name)) {
        return std::nullopt;
    } else {
        size_t hash = get_hash(name);
        table[hash].push_back(lexeme(name));
        return std::optional { place(hash, table[hash].size() - 1) };
    }
}

std::optional<place> var_table::find_in_table (const std::string& name) const {
    size_t hash = get_hash(name);
    using _Iter = std::vector<lexeme>::const_iterator;
    _Iter it = std::find(table[hash].begin(), table[hash].end(), lexeme(name));

    return it == table[hash].end()
            ? std::nullopt
            : std::optional { place(hash, std::distance(table[hash].begin(), it)) };
}

inline size_t var_table::get_hash (const std::string& name) const {
    return std::hash<std::string>{ }(name) % get_size_table(); }

var_table& var_table::set_type (const place& plc, TYPE type) {
    using enum ::place::POS;
    table[plc(ROW)][plc(COLLUMN)].set_type(type);
    return *this;
}

var_table& var_table::set_value (const place& plc, bool value) {
    using enum ::place::POS;
    table[plc(ROW)][plc(COLLUMN)].set_value(value);
    return *this;
}

std::vector<std::vector<lexeme>>::const_iterator var_table::begin () const {
    return table.begin(); }

std::vector<std::vector<lexeme>>::const_iterator var_table::end () const {
    return table.end(); }

std::ostream& operator<< (std::ostream& out, const var_table& _tab) {
    auto type_to_str =
        [](TYPE _type) -> std::string {
            return
                _type == TYPE::UNDEFINED
                    ? "undefined"
                    : _type == TYPE::INT
                        ? "int" : "char"; };

    auto bool_to_str =
        [](bool _init) -> std::string {
            return _init == false
                ? "false" : "true" ; };

    tabulate::Table movies;

    movies.add_row({ "hash", "id", "name", "type", "init" });
    for (auto iter = _tab.begin(); iter != _tab.end(); iter++) {
        size_t collumn = 0;
        for (auto it = iter->begin(); it  != iter->end(); it++, collumn++) {
            std::string _name = it->get_name();
            movies.add_row({
                std::to_string(_tab.get_hash(_name)),
                std::to_string(collumn),
                _name,
                type_to_str(it->get_type()),
                bool_to_str(it->get_init()) });
        }
    }

    using namespace ::tabulate;
    movies.column(0).format().font_align(FontAlign::center).width(8);
    movies.column(1).format().font_align(FontAlign::center).width(8);
    movies.column(2).format().font_align(FontAlign::center).width(32);
    movies.column(3).format().font_align(FontAlign::center).width(15);
    movies.column(4).format().font_align(FontAlign::center).width(13);

    return out << movies << '\n';
}
#endif /// _VAR_TABLE_HPP