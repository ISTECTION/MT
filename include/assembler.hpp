#ifndef _ASSEMBLER_HPP
#define _ASSEMBLER_HPP
#include "parse.hpp"

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <cctype>
#include <stack>

/**
 * @brief Класс переводящий С++ в Ассемблерный код из обратной польской записи
 */
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
            ? processing_postfix(fin)
            : assert(print_error(std::filesystem::canonical(_postfix_path).string()));
        fin.close();

        write_file();
    }

private:
    /**
     * @brief Подпрограмма перевода обратной польской записи в Ассемблерный код
     *                w
     */
    auto processing_postfix (std::ifstream& ) -> void;

    /**
     * @brief Функция записи Ассемблерного кода в файл `assembler.asm`
     *
     */
    auto write_file () -> void;
};

auto assembler::processing_postfix (std::ifstream& _ifstream_postfix) -> void {
    using enum ::place::POS;

    auto type_to_string_assembly =
    [](TYPE _type) -> std::string {
        return _type == TYPE::INT
            ? "dd"
            : "dw" ;
    };

    auto _bitw_shift =
    [](char _bit) -> std::string {
        return _bit == '<'
            ? "SHL"
            : "SHR" ;
    };

    auto _bitw_opers =
    [](char _bit) -> std::string {
        return _bit == '&'
            ? "AND"
            : "OR" ;
    };

    auto _logic_opers =
    [](const std::string& _bit) -> std::string {
        using namespace _switch::literals;
        switch (_switch::hash(_bit))
        {
            case "<"_hash:  return "JG";  break;
            case ">"_hash:  return "JL";  break;
            case "!="_hash: return "JE";  break;
            case "=="_hash: return "JNE"; break;

            default: return std::string { };
        }
    };

    os_assembly_code << std::setw(80) << std::left << ".386"                        << ' ' << "; разрешает ассемблирование непривилегированных инструкций процессора 80386" << '\n';
    os_assembly_code << std::setw(80) << std::left << ".MODEL FLAT, STDCALL"                                                << '\n' << '\n';

    os_assembly_code << std::setw(80) << std::left << "EXTRN ExitProcess@4:PROC"    << ' ' << "; выход"                     << '\n' << '\n';
    os_assembly_code << std::setw(80) << std::left << ".DATA"                       << ' ' << "; сегмент данных"            << '\n';

    os_assembly_code << std::setw(80) << std::left << "tmp_var      dd ?"           << ' ' << "; переменная, для преобразования типов"                                     << '\n';
    os_assembly_code << std::setw(80) << std::left << "tmp_bit      dd ?"           << ' ' << "; переменная, для операций побитового сдвига"                               << '\n';


    /// Объявление переменных
    for (auto iter = this->identifiers.begin(); iter != this->identifiers.end(); iter++) {
        for (auto it = iter->begin(); it  != iter->end(); it++) {
            std::string_view type_id = type_to_string_assembly(it->get_type());
            os_assembly_code << std::setw(12) << std::left << it->get_name() << ' ' << type_id << ' ' << '?' << '\n';
        }
    }

    os_assembly_code << '\n';
    /// Объявление констант
    for (auto iter = this->constants.begin(); iter != this->constants.end(); iter++) {
        for (auto it = iter->begin(); it  != iter->end(); it++) {
            place _pl_const = this->constants.find_in_table(it->get_name()).value();

            std::ostringstream _constant_name;
            _constant_name << "const_" << _pl_const(ROW) << '_' << _pl_const(COLLUMN);
            os_assembly_code << std::setw(16) << std::left << _constant_name.str()  << ' ' << "dd" << ' ' << it->get_name() << '\n';
        }
    }

    os_assembly_code << '\n';
    os_assembly_code << std::setw(80) << std::left << ".CODE"     << ' ' << "; сегмент кода"      << '\n';
    os_assembly_code << std::setw(80) << std::left << "MAIN PROC" << ' ' << "; метка точки входа" << '\n';
    os_assembly_code << std::setw(80) << std::left << "FINIT"     << ' '                  << '\n' << '\n';

    std::string _record;
    std::size_t _logic_count = 0;
    /// Пара, где bool хранит статус присваивания, а в строке хранится
    /// идентификатор, которому нужно присвоить вычисляемое значение
    auto _id_var_asm = std::pair<bool, std::string>(false, std::string { });

    /// Обработка обратной польской записи
    while (_ifstream_postfix >> _record) {

        /// Если это число (положительное : отрицательное)
        if (std::isdigit(_record.front()) || (
                _record.front() == '-' &&
                _record.length() > 1   &&
                std::isdigit(_record.back()) )
                ) {

            place _pl_const = constants.find_in_table(_record).value();
            os_assembly_code << "FILD" << ' ' << "const_" << _pl_const(ROW) << '_' << _pl_const(COLLUMN) << '\n';
        }

        /// Если это идентификатор
        if (std::isalpha(_record.front()) || _record.front() == '_') {
            if (_id_var_asm.first == true) {
                os_assembly_code << "FILD" << ' ' << _record << '\n'; }

            /// Только если это идентификатор, которому нужно присвоить значение
            if (_id_var_asm.first == false) { _id_var_asm = std::make_pair(true, _record); }
        }

        if (_record.front() == '+') { os_assembly_code << "FADD" << ' ' << '\n'; }
        if (_record.front() == '*') { os_assembly_code << "FMUL" << ' ' << '\n'; }

        /// Только если это не унарный минус
        if ((_record.front() == '-' && _record.length() == 1) || _record == "-=") {
            os_assembly_code << "FSUB" << ' ' << '\n';
        }

        if (((_record.front() == '<' || _record.front() == '>') && _record.length() == 1) || _record == "==" || _record == "!=") {
            os_assembly_code << "FCOMPP"               << '\n';
            os_assembly_code << "FSTSW" << ' ' << "AX" << '\n';
            os_assembly_code << "SAHF"                 << '\n';
            os_assembly_code << _logic_opers(_record) << ' ' << "JMP_ZERO_" + std::to_string(_logic_count) << '\n';
            os_assembly_code << '\t' << "FLD1" << '\n';
            os_assembly_code << "JMP" << ' ' << "JMP_END_" + std::to_string(_logic_count) << '\n';
            os_assembly_code << "JMP_ZERO_" + std::to_string(_logic_count) << ':' << '\n';
            os_assembly_code << '\t' << "FLDZ" << '\n';
            os_assembly_code << "JMP_END_"  + std::to_string(_logic_count) << ':' << '\n';
            _logic_count++;
        }

        if (_record == "<<" || _record == ">>") {
            os_assembly_code << "FISTP" << ' ' << "tmp_bit" << '\n';
            os_assembly_code << "FISTP" << ' ' << "tmp_var" << '\n';

            os_assembly_code << "MOV"                        << ' ' << "ECX"     << ',' << ' ' << "tmp_bit" << '\n';
            os_assembly_code << _bitw_shift(_record.front()) << ' ' << "tmp_var" << ',' << ' ' << "CL"      << '\n';
            os_assembly_code << "FILD"                       << ' ' << "tmp_var"                            << '\n';
        }

        if ((_record.front() == '&' || _record.front() == '|') && _record.length() == 1) {
            os_assembly_code << "FISTP" << ' ' << "tmp_var"                        << '\n';
            os_assembly_code << "MOV"   << ' ' << "ECX" << ',' << ' ' << "tmp_var" << '\n';

            os_assembly_code << "FISTP"                      << ' ' << "tmp_var"                        << '\n';
            os_assembly_code << _bitw_opers(_record.front()) << ' ' << "tmp_var" << ',' << ' ' << "ECX" << '\n';
            os_assembly_code << "FILD"                       << ' ' << "tmp_var"                        << '\n';
        }

        if (_record == "-=" || _record == "+=") {
            os_assembly_code << "FISTP" << ' ' << _id_var_asm.second << '\n' << '\n';
            _id_var_asm.first = false;
        }

        if (_record == "&=" || _record == "|=") {
            os_assembly_code << "FISTP"                      << ' ' << "tmp_var"                                     << '\n';
            os_assembly_code << "MOV"                        << ' ' << "ECX"              << ',' << ' ' << "tmp_var" << '\n';
            os_assembly_code << _bitw_opers(_record.front()) << ' ' << _id_var_asm.second << ',' << ' ' << "ECX"     << '\n' << '\n';
            _id_var_asm.first = false;
        }

        if (_record == "<<=" || _record == ">>=") {
            os_assembly_code << "FISTP"                      << ' ' << "tmp_bit"                                     << '\n';
            os_assembly_code << "MOV"                        << ' ' << "ECX"              << ',' << ' ' << "tmp_bit" << '\n';
            os_assembly_code << _bitw_shift(_record.front()) << ' ' << _id_var_asm.second << ',' << ' ' << "CL"      << '\n' << '\n';
            _id_var_asm.first = false;
        }

        /// Если это знак равенства
        /// (Проверка на длину необходима в случае операции сравнения: ==)
        if (_record.front() == '=' && _record.length() == 1) {
            os_assembly_code << "FISTP" << ' ' << _id_var_asm.second << '\n' << '\n';
            _id_var_asm.first = false;
        }
    }

    os_assembly_code << std::setw(80) << std::left << "PUSH 0" << ' ' << "; параметр: код выхода" << '\n';
    os_assembly_code << "CALL ExitProcess@4"                                                      << '\n';
    os_assembly_code << "MAIN ENDP"                                                               << '\n';
    os_assembly_code << "END MAIN";
}

auto assembler::write_file () -> void {
    std::ofstream fout(this->get_parrent_path() / "assembler.asm");
    fout << os_assembly_code.str();
    fout.close();
}
#endif /// _ASSEMBLER_HPP