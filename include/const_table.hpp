#ifndef _CONST_TABLE_HPP
#define _CONST_TABLE_HPP
#include "tabulate/tabulate.hpp"

#include <type_traits>
#include <filesystem>
#include <iostream>
#include <optional>
#include <fstream>
#include <string>
#include <set>

namespace Path_Const_Table {
    const std::filesystem::path
    ///    letters     = "file/const/table_letters.txt",
    ///    digits     = "file/const/table_digits.txt",
        keywords    = "file/const/table_keywords.txt",
        operations  = "file/const/table_operations.txt",
        separators  = "file/const/table_separators.txt";
}

/**
 * @brief Шаблонный класс хранящий константные таблицы
 */
template <typename _Type>
class const_table
{
private:
    std::set<_Type> table;

public:
    explicit const_table (const std::filesystem::path& _path) {
        std::ifstream fin(_path);
        bool is_open = read_file(fin);

        if (is_open == false)
            std::cerr
                << "file not open: "
                << _path.string().c_str()
                << '\n';
    }

    /**
     * @brief Получить индекс элемента
     *
     * @param elem Элемент для поиска
     * @return int Индекс элемента (-1 если элемент не был найден)
     */
    int get_num (_Type elem) const;

    /**
     * @brief Проверка существования элемента
     *
     * @param elem Элемент для поиска
     * @return true Если элемент найден
     * @return false Если элемент не найден
     */
    bool contains (_Type elem) const;

    /**
     * @brief Получить элемент по его индексу
     *
     * @param num Индекс элемента
     * @return std::optional<_Type> Будет иметь значение std::nullopt, если элемент не найден
     */
    std::optional<_Type> get_elem (size_t num) const;

    /**
     * @brief Выводит таблицу в поток
     */
    template <typename V>
    friend std::ostream& operator<< (std::ostream& out, const const_table<V>& _tbl);

    /**
     * @brief Оператор возвращает индекс элемента
     *
     * @param elem Элемент для поиска
     * @return int Индекс элемента (-1 если элемент не был найден)
     */
    int operator[] (_Type elem) const noexcept;

    /**
     * @brief Возвращает константный итератор на начало таблицы
     */
    typename std::set<_Type>::const_iterator begin () const;

    /**
     * @brief Возвращает константный итератор на конец таблицы
     */
    typename std::set<_Type>::const_iterator end () const;

private:

    /**
     * @brief Функция чтения файла
     */
    bool read_file (std::ifstream& );

    /**
     * @brief Добавляет элемент в таблицу
     * @param elem Элемент, который нужно добавить
     */
    inline void add (_Type elem);
};

template <typename _Type>
typename std::set<_Type>::const_iterator const_table<_Type>::begin () const {
    return table.begin();
}

template <typename _Type>
typename std::set<_Type>::const_iterator const_table<_Type>::end () const {
    return table.end();
}

template <typename _Type>
bool const_table<_Type>::contains (_Type elem) const {
    using _Iter = typename std::set<_Type>::iterator;
    _Iter it = table.find(elem);
    return it == table.end()
        ? false
        : true ;
}

template <typename _Type>
int const_table<_Type>::get_num (_Type elem) const {
    using _Iter = typename std::set<_Type>::iterator;
    _Iter it = table.find(elem);
    return it == table.end()
        ? -1
        : std::distance(table.begin(), it);
}

template <typename _Type>
std::optional<_Type> const_table<_Type>::get_elem (size_t num) const {
    using _Iter = typename std::set<_Type>::iterator;
    if (num < table.size()) {
        _Iter it = table.begin();
        std::advance(it, num);
        return std::optional { *it };
    } else
        return std::nullopt;
}

template <typename _Type>
bool const_table<_Type>::read_file (std::ifstream& _istream) {
    auto read = [this](std::ifstream& _istream) -> bool {
        for (_Type elem; _istream >> elem; add(elem));
        return true; };
    return _istream.is_open() ? read(_istream) : false;
}

template <typename _Type>
inline void const_table<_Type>::add (_Type elem) {
    table.insert(elem);
}

template <typename _Type>
int const_table<_Type>::operator[] (_Type elem) const noexcept {
    return get_num(elem);
}

template <typename V>
std::ostream& operator<< (std::ostream& out, const const_table<V>& _tbl) {
    using _Iter = typename std::set<V>::const_iterator;

    tabulate::Table movies;
    if(std::is_same<V, std::string>::value ) {
        _Iter it = _tbl.begin();
        for (size_t pos = 0; it != _tbl.end(); pos++) {
            movies.add_row({
                    std::to_string(pos),
                    std::string(*it) });
            ++it;
        }
    }
    else {
        std::cerr << "This type is not processed" << '\n';
        std::exit(2); }

    movies.column(0).format().font_align(tabulate::FontAlign::center).width(8);
    movies.column(1).format().font_align(tabulate::FontAlign::center).width(32);
    return out << movies << '\n';
}
#endif /// _CONST_TABLE_HPP