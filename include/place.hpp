#ifndef _PLACE_HPP
#define _PLACE_HPP

/**
 * @brief Класс для хранения позиции элемента в двумерном массиве
 */
class place
{
private:
    size_t i;
    size_t j;

public:
    explicit place(size_t _i, size_t _j) : i(_i), j(_j) { }

    /// Индекс
    enum class POS {
        ROW,        //< Строка
        COLLUMN     //< Столбца
    };
    /**
     * @brief
     *
     * @param _pos
     * @return size_t Индекс строки или столбца в зависимости от выбора _pos
     */
    size_t operator() (POS _pos) const noexcept;

    /**
     * @brief Перегрузка оператора `operator<<` для вывода индексов в поток
     *
     * @param out   Поток (Например: std::cout, std::ofstream)
     * @param _plc  Екземляр класса, который будем выводить
     * @return std::ostream&
     */
    friend std::ostream& operator<< (std::ostream& out, const place& _plc);
};

size_t place::operator() (POS _pos) const noexcept {
    return _pos == POS::ROW ? i : j ; }

std::ostream& operator<< (std::ostream& out, const place& _plc) {
    using enum ::place::POS;
    return out
            << _plc(ROW)     << ' '
            << _plc(COLLUMN) << '\n'; }
#endif /// _PLACE_HPP