#ifndef _CONST_TABLE_HPP
#define _CONST_TABLE_HPP

#include <filesystem>
#include <iostream>
#include <fstream>
#include <set>

template <typename _Type>
class const_table
{
private:
    std::set<_Type> table;

public:
    explicit const_table (const std::filesystem::path& _path) {
        std::ifstream fin(_path);
        if (fin.is_open()) {
            read_file(fin);
        } else {
            constexpr size_t error = 4;
            std::cerr << error << ": file not open" << '\n';
        }
    }

    const_table (const const_table& ) = delete;
    const_table (const_table&& )      = delete;

    const_table& operator= (const const_table& ) = delete;
    const_table& operator= (const_table&& )      = delete;

    inline void add (_Type elem) { table.insert(elem); }

    bool contains (_Type elem) {
        using _Iter = typename std::set<_Type>::iterator;
        _Iter it = table.find(elem);
        return it == table.end()
            ? false
            : true;
    }

    int get_num (_Type elem) {
        using _Iter = typename std::set<_Type>::iterator;
        _Iter it = table.find(elem);
        return it == table.end()
            ? -1
            : std::distance(table.begin(), it);
    }

    _Type get_elem (size_t num) {
        return (num < 0 || num >= table.size())
            ? -1
            : *(table.begin() + num);
    }

private:
    void read_file (std::ifstream& istream) {
        _Type elem;
        while (!istream.eof()) {
            istream >> elem;
            add(elem);
        }
    }
};
#endif /// _CONST_TABLE_HPP