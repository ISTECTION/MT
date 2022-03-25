#ifndef _ASSEMBLER_HPP
#define _ASSEMBLER_HPP
#include "parse.hpp"

#include <cassert>

class assembler : public parse
{
private:


public:
    explicit assembler(const std::filesystem::path& _inp)
        : parse(_inp) {

        if (parse::syntax_fail()) {
            std::cerr
                << "generate error file: "
                << (_inp.parent_path() / "syntactic_error.txt").string()
                << '\n';
            assert(false);
        }

        std::filesystem::path _postfix_path = this->get_parrent_path() / "postfix.txt";
        std::ifstream fin(_postfix_path);
        fin.is_open()
            ? read_postfix(fin)
            : assert(print_error(std::filesystem::canonical(_postfix_path).string()));
        fin.close();
    }

private:
    auto read_postfix (std::ifstream& ) -> void;

};

auto assembler::read_postfix (std::ifstream& _ifstream_postfix) -> void {



}

#endif /// _ASSEMBLER_HPP