#ifndef _LEXEME_HPP
#define _LEXEME_HPP
#include <string>

/// Возможные типы идентификаторов
enum class TYPE {
    UNDEFINED,  //< Не установленно
    INT,        //< int
    CHAR        //< char
};

/**
 * @brief Класс хранения лексемы
 */
class lexeme
{
private:
    std::string name;

    /// Тип идентификатора
    /// 0 - Не определён
    /// 1 - int
    /// 2 - char
    TYPE type;

    /// Параметр отвечающий за инициализацию идентификатора
    /// true  - инициализирован
    /// false - не инициализирован
    bool init;

public:
    explicit lexeme(std::string _name)
        : name(_name),
          type(TYPE::UNDEFINED),
          init(false) { }

    /**
     * @brief Устанавить тип объекту
     * @param _type Может принимать значения enum TYPE
     */
    void set_type (TYPE _type) { type = _type; }
    /**
     * @brief Устанавить значение объекту
     * @param _init Принимает значение типа bool (true - если значение установленно)
     */
    void set_value(bool _init) { init = _init; }

    /**
     * @brief Получить имя объекта
     * @return std::string Имя объекта
     */
    std::string get_name () const { return name; }
    /**
     * @brief Получить тип объекта
     * @return TYPE Тип объекта
     */
    TYPE get_type () const { return type; }
    /**
     * @brief Получить значение объекта
     *
     * @return true  Если объект был инициализирован
     * @return false Если объект не был инициализирован
     */
    bool get_init () const { return init; }

    // Для std::find
    /**
     * @brief Перегрузка оператора пользовательского класса
     *
     * @param _lhs Левый класс от знака == в цепочке сравнений
     * @param _rhs Правый класс от знака == в цепочке сравнений
     * @return true  Если значения переменной name классов совпадают
     * @return false Если значения переменной name классов отличаются
     */
    friend bool operator== (const lexeme& _lhs, const lexeme& _rhs);
};

bool operator== (const lexeme& _lhs, const lexeme& _rhs) {
    return
        _lhs.name == _rhs.name;
}
#endif /// _LEXEME_HPP