#ifndef _LEXEME_HPP
#define _LEXEME_HPP
#include <string>

class lexeme
{
private:
    std::string name;

    /// Тип идентификатора
    /// 0 - Не определён
    /// 1 - int
    size_t type;

    /// Параметр отвечающий за инициализацию идентификатора
    /// true  - инициализирован
    /// false - не инициализирован
    bool value;

public:
    explicit lexeme(std::string _name)
        : name(_name),
          type(0),
          value(false) { }

    void set_type (size_t _type) { type  = _type;  }
    void set_value (bool _value) { value = _value; }

    // Для std::find
    friend bool operator== (const lexeme& _lhs, const lexeme& _rhs);
};

bool operator== (const lexeme& _lhs, const lexeme& _rhs) {
    return
        _lhs.name == _rhs.name;
}
#endif /// _LEXEME_HPP