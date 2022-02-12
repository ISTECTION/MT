#include <gtest/gtest.h>

#include "const_table.hpp"
#include "var_table.hpp"

#include <filesystem>
#include <string>

TEST(ConstTable, readFile) {

    using ::std::filesystem::path;
    using ::std::string;

    const_table<string> key_table(path("file/keywords.txt"));

    ASSERT_EQ(key_table.contains("int"), true);
    ASSERT_EQ(key_table.contains("main"), true);
    ASSERT_EQ(key_table.contains("float"), true);
    ASSERT_EQ(key_table.contains("return"), true);

    ASSERT_EQ(key_table.contains("typename"), false);

    ASSERT_EQ(key_table.get_num("float"),  0);
    ASSERT_EQ(key_table.get_num("int"),    1);
    ASSERT_EQ(key_table.get_num("return"), 3);
}