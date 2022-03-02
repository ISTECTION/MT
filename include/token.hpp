#ifndef _TOKEN_HPP
#define _TOKEN_HPP
#include <iostream>
#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e); }

enum class TABLE {
    OPERATION = 1,      ///< Таблицы операций
    KEYWORDS,           ///< Таблица ключевых слов
    SEPARATORS,         ///< Таблица разделителей
    IDENTIFIERS,        ///< Таблица идентификаторов
    CONSTANTS   };      ///< Таблица констант

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
        : table(_table), i(_i), j(_j) { }

    /**
     * @brief Перегрузка оператора `operator<<` для вывода токена в поток
     *
     * @param out  Поток (Например: std::cout, std::ofstream)
     * @param _tkn Екземляр класса, который будем выводить
     * @return std::ostream&
     */

    friend std::ostream& operator<< (std::ostream& out, const token& _tkn);
};

std::ostream& operator<< (std::ostream& out, const token& _tkn) {
    return out
            << to_underlying(_tkn.table) << ' '
            << _tkn.i                    << ' '
            << _tkn.j                    << '\n';
}

#endif /// _TOKEN_HPP