#ifndef _ASSEMBLER_HPP
#define _ASSEMBLER_HPP
#include "parse.hpp"

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>

class assembler : public parse
{
private:
    std::ostringstream os_assembly_code;        ///< Поток с текстом программы переведнным в ассемблерный код

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
            ? generate_postfix(fin)
            : assert(print_error(std::filesystem::canonical(_postfix_path).string()));
        fin.close();

        write_file();
    }

private:
    auto generate_postfix (std::ifstream& ) -> void;

    auto write_file () -> void;
};

auto assembler::generate_postfix (std::ifstream& _ifstream_postfix) -> void {

    auto type_to_string_assembly =
    [](TYPE _type) -> std::string {
        return _type == TYPE::INT
            ? "dd"
            : "db" ;
    };

    os_assembly_code << std::setw(80) << std::left << ".386"                        << ' ' << "; разрешает ассемблирование непривилегированных инструкций процессора 80386" << '\n';
    os_assembly_code << std::setw(80) << std::left << ".MODEL FLAT, STDCALL"                                                                        << '\n' << '\n';

    os_assembly_code << std::setw(80) << std::left << "EXTRN ExitProcess@4:PROC"    << ' ' << "; выход"                                             << '\n' << '\n';
    os_assembly_code << std::setw(80) << std::left << ".DATA"                       << ' ' << "; сегмент данных"                                    << '\n';

    /// Объявление переменных
    for (auto iter = this->identifiers.begin(); iter != this->identifiers.end(); iter++) {
        for (auto it = iter->begin(); it  != iter->end(); it++) {
            os_assembly_code << std::setw(8) << std::left << it->get_name() << ' '
            << type_to_string_assembly(it->get_type()) << ' ' << '?' << '\n';
        }
    }

    os_assembly_code << '\n';
    /// Объявление констант
    for (auto iter = this->constants.begin(); iter != this->constants.end(); iter++) {
        for (auto it = iter->begin(); it  != iter->end(); it++) {
            std::ostringstream _constant_name;
            _constant_name << "const_" << 0 << '_' << 0;
            os_assembly_code << std::setw(16) << std::left << _constant_name.str()  << ' ' << "dd" << ' ' << it->get_name() << '\n';
        }
    }

    os_assembly_code << '\n';
    os_assembly_code << std::setw(80) << std::left << ".CODE"                       << ' ' << "; сегмент кода"                                      << '\n';
    os_assembly_code << std::setw(80) << std::left << "MAIN PROC"                   << ' ' << "; метка точки входа"                                 << '\n';

}

auto assembler::write_file () -> void {
    std::ofstream fout(this->get_parrent_path() / "assembler.asm");
    fout << os_assembly_code.str();
    fout.close();
}
#endif /// _ASSEMBLER_HPP