#include <gtest/gtest.h>

#include "const_table.hpp"
#include "var_table.hpp"

#include <filesystem>
#include <optional>
#include <string>

using ::std::filesystem::path;
using ::std::string;

/// ~~~~~ LEXEME ~~~~~ ///
TEST(Lexeme, operator) {
    lexeme _l("FIO");
    ASSERT_EQ(_l == lexeme("FIO"), true);
}
/// ~~~~~ LEXEME ~~~~~ ///


/// ~~~~~ CONST_TABLE ~~~~~ ///
TEST(ConstTable, contains) {
    const_table<string> key_table(path_const_table::keywords);

    ASSERT_EQ(key_table.contains("int"),  true);
    ASSERT_EQ(key_table.contains("char"), true);
    ASSERT_EQ(key_table.contains("main"), true);

    ASSERT_EQ(key_table.contains("typename"), false);
}

TEST(ConstTable, get_num) {
    const_table<string> key_table(path_const_table::keywords);

    ASSERT_EQ(key_table.get_num("char"),   0);
    ASSERT_EQ(key_table.get_num("int"),    1);
    ASSERT_EQ(key_table.get_num("return"), 3);

    ASSERT_EQ(key_table.get_num("float"), -1);
}

TEST(ConstTable, get_elem) {
    const_table<string> key_table(path_const_table::keywords);

    std::optional<std::string> _opt = key_table.get_elem(3);
    ASSERT_EQ(_opt.value(), "return");

    _opt = key_table.get_elem(8);
    ASSERT_EQ(_opt, std::nullopt);
}
/// ~~~~~ CONST_TABLE ~~~~~ ///


/// ~~~~~ VAR_TABLE ~~~~~ ///

TEST(VarTable, add) {
    var_table var_tab;
    std::optional<place> pl = var_tab.add("name");
    ASSERT_EQ(pl.has_value(), 1);

    std::optional<lexeme> lex = var_tab.get_lexeme(pl.value());
    ASSERT_EQ(lex.has_value(), 1);

    ASSERT_EQ(lex.value().get_name(), "name");
    ASSERT_EQ(lex.value().get_init(), false);
    ASSERT_EQ(lex.value().get_type(), TYPE::UNDEFINED);
}

TEST(VarTable, contains) {
    var_table var_tab;
    var_tab.add("name");
    ASSERT_EQ(var_tab.contains("name"), 1);
}

TEST(VarTable, find_in_table) {
    var_table var_tab;
    std::optional<place> _plc1 = var_tab.add("name");
    std::optional<place> _plc2 = var_tab.find_in_table("name");

    using Pos = ::place::POS;
    ASSERT_EQ(_plc1.value()(Pos::ROW), _plc2.value()(Pos::ROW));
    ASSERT_EQ(_plc1.value()(Pos::COLLUMN), _plc2.value()(Pos::COLLUMN));
}

TEST(VarTable, get_size_table) {
    var_table var_tab;
    ASSERT_EQ(var_tab.get_size_table(), 53);

    var_table var_tab_custom(128);
    ASSERT_EQ(var_tab_custom.get_size_table(), 128);
}

TEST(VarTable, set_type) {
    var_table var_tab;
    std::optional<place> plc = var_tab.add("lenght");
    var_tab.set_type(plc.value(), TYPE::INT);

    std::optional<lexeme> lex = var_tab.get_lexeme(plc.value());
    ASSERT_EQ(lex.value().get_type(), TYPE::INT);
}

TEST(VarTable, set_value) {
    var_table var_tab;
    std::optional<place> plc = var_tab.add("lenght");
    var_tab.set_value(plc.value(), true);

    std::optional<lexeme> lex = var_tab.get_lexeme(plc.value());
    ASSERT_EQ(lex.value().get_init(), true);
}

TEST(VarTable, set_value_type) {

    var_table _tbl;
    std::optional<place> _opt1 = _tbl.add("a");

    _tbl.set_type(_opt1.value(), TYPE::INT)
        .set_value(_opt1.value(), true);

    std::optional<lexeme> lex = _tbl.get_lexeme(_opt1.value());
    ASSERT_EQ(lex.value().get_name(), "a");
    ASSERT_EQ(lex.value().get_type(), TYPE::INT);
    ASSERT_EQ(lex.value().get_init(), true);
}

/// ~~~~~ VAR_TABLE ~~~~~ ///