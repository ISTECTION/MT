#ifndef _TOKEN_HPP
#define _TOKEN_HPP
#include "place.hpp"

#include <iostream>         /// std::ostream
#include <stdexcept>        /// std::runtime_error
#include <type_traits>      /// std::underlying_type_t


template <typename E>
constexpr auto to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

enum class TABLE {
    OPERATION = 1,      ///< Таблицы операций
    KEYWORDS,           ///< Таблица ключевых слов
    SEPARATORS,         ///< Таблица разделителей
    IDENTIFIERS,        ///< Таблица идентификаторов
    CONSTANTS,          ///< Таблица констант

    NOT_DEFINED = -1
};

/**
 * @brief Класс для хранения номера таблицы и позиции ключевого слова в ней
 */
class token
{
private:
    TABLE table;
    size_t i;
    int    j;

public:
    explicit token(TABLE _table, size_t _i, int _j)
        : table(_table),
          i(_i),
          j(_j) { }

    explicit token()
        : table(TABLE::NOT_DEFINED),
          i(0),
          j(0) { }

    explicit token(const std::string& text_token) {
        std::istringstream _istream { text_token };
        std::string _table, _i, _j;

        _istream.seekg(1);
        std::getline(_istream, _table, ',');
        std::getline(_istream, _i,     ',');
        std::getline(_istream, _j,     ')');

        table = static_cast<TABLE>(std::stoi(_table));
        i = static_cast<std::size_t>(std::stoi(_i));
        j = std::stoi(_j);
    }

    /**
     * @brief Возвращает номер таблицы
     *
     * @return TABLE Enum с названиями таблиц
     */
    TABLE get_table () const { return table; }

    /**
     * @brief Возвращает номер строки
     *
     * @return size_t Исключительно положительное число
     */
    size_t get_row () const { return i; }

    /**
     * @brief Возвращает номер колонки
     *
     * @return int Может принимать значени -1, для постоянных таблиц
     */
    int get_column () const { return j; }

    /**
     * @brief Возвращает класс place содержащий значение строки и колонки лексемы
     *
     * @return place Класс хранящий 2 значения
     */
    place get_place () const {
        if (table == TABLE::IDENTIFIERS || table == TABLE::CONSTANTS)
            return place { i, static_cast<std::size_t>(j) };
        else throw std::runtime_error("table != (TABLE::IDENTIFIERS | TABLE::CONSTANTS)");
    }

    /**
     * @brief Перегрузка оператора `operator<<` для вывода токена в поток
     *
     * @param out  Поток (Например: std::cout, std::ofstream)
     * @param _tkn Екземляр класса, который будем выводить
     * @return std::ostream&
     */
    friend std::ostream& operator<< (std::ostream& out, const token& _tkn);

    /**
     * @brief Перешрузка оператора `operator>>` для ввода токена из потока
     *
     * @param in   Поток (Например: std::cin, std::ifstream)
     * @param _tkn Екземляр класса, в который будем записывать токен
     * @return std::istream&
     */
    friend std::istream& operator>> (std::istream& in, token& _tkn);
};

std::ostream& operator<< (std::ostream& out, const token& _tkn) {
    return out << '(' << to_underlying(_tkn.table) << ", " << _tkn.i << ", " << _tkn.j << ')';
}

std::istream& operator>> (std::istream& in, token& _tkn) {
    size_t table;
    in >>  table;
    in >> _tkn.i;
    in >> _tkn.j;

    _tkn.table = static_cast<TABLE>(table);
    return in;
}
#endif /// _TOKEN_HPP