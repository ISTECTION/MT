#ifndef _TOKEN_HPP
#define _TOKEN_HPP
#include <iostream>


/**
 * @brief Класс для хранения номера таблицы и позиции ключевого слова в ней
 */
class token
{
private:
    size_t table;
    size_t i;
    int    j;

public:
    explicit token(size_t _table, size_t _i, int _j)
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
            << _tkn.table << ' '
            << _tkn.i     << ' '
            << _tkn.j     << '\n';
}

#endif /// _TOKEN_HPP