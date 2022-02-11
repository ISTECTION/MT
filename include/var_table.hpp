#ifndef _VAR_TABLE_HPP
#define _VAR_TABLE_HPP
#include "lexeme.hpp"

#include <vector>
#include <set>

class var_table
{
private:
    std::vector<std::vector<lexeme>> table;

public:
    explicit var_table(size_t _size = 26 * 2 + 10) {
        table.resize(_size);
    }

    var_table (const var_table& ) = delete;
    var_table (var_table&& )      = delete;

    var_table& operator= (const var_table& ) = delete;
    var_table& operator= (var_table&& )      = delete;

    bool add(const std::string& name);

    bool contains(const std::string& name) const;
    int  find(const std::string& name) const;

    bool set_type (const std::string& name, size_t type);
    bool set_value(const std::string& name, bool value);

    inline size_t get_size_table() const { return table.size(); }
private:
    inline size_t get_hash(const std::string& name) const {
        return
            std::hash<std::string>{ }(name) % get_size_table();
    }
};

bool var_table::add(const std::string& name) {

    if (contains(name)) return false;
    else {
        size_t hash = get_hash(name);

        table[hash].push_back(lexeme(name));
        return true;
    }
}

bool var_table::contains(const std::string& name) const {
    return find(name) < 0
        ? false
        : true;
}

int var_table::find(const std::string& name) const {
    size_t hash = get_hash(name);
    lexeme find_t(name);
    using _Iter = std::vector<lexeme>::const_iterator;
    _Iter it =
        std::find(
            table[hash].begin(), table[hash].end(), find_t);

    return it == table[hash].end()
            ? -1
            : std::distance(table[hash].begin(), it);
}

bool var_table::set_type(const std::string& name, size_t type) {
    if (contains(name)) {
        size_t hash = get_hash(name);

        int _pos = find(name);
        table[hash][_pos].set_type(type);

        return true;
    } else
        return false;
}

bool var_table::set_value(const std::string& name, bool value) {

    if (contains(name)) {
        size_t hash = get_hash(name);

        int _pos = find(name);
        table[hash][_pos].set_value(value);

        return true;
    } else
        return false;
}
#endif /// _VAR_TABLE_HPP