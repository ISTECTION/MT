#include "argparse/argparse.hpp"

#include "translator.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, const char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    argparse::ArgumentParser _prs("MT", "1.0.0");
    _prs.add_argument("-i", "--input")
        .help( "path to input files" )
        .required();

    try {
        using ::std::filesystem::path;
        _prs.parse_args(argc, argv);

        path _inp = _prs.get<std::string>("-i");

        const_table<std::string> keywords(Path_Const_Table::keywords);
        std::cout << "contains (int): " << keywords.contains("int") << '\n';
        int i1 = keywords.get_num("int");

        std::cout << "get_num (int): " << i1 << '\n';
        std::cout << "get_elem "
            << i1 << ": "
            << keywords.get_elem(i1).has_value() << '\n';

        std::cout << "get_elem "
            << 8 << ": "
            << keywords.get_elem(8).has_value() << '\n';

        std::ofstream fout("table1.txt");
        fout << keywords;
        fout.close();

        var_table var_tab;
        std::optional<place> pl = var_tab.add("name");
        if (pl != std::nullopt)
            std::cout << "place (\"name\"): "     << pl.value();

        pl = var_tab.add("position");
        if (pl != std::nullopt)
            std::cout << "place (\"position\"): " << pl.value();

        std::cout
            << "get_hash (\"position\"): "
            << var_tab.get_hash("position") << '\n';

        std::optional<place> pl0 = var_tab.add("a");
        std::optional<place> pl1 = var_tab.add("b");
        std::optional<place> pl2 = var_tab.add("lenght");
        std::optional<place> pl3 = var_tab.add("width");

        var_tab.set_type(pl0.value(), TYPE::CHAR);
        var_tab.set_type(pl1.value(), TYPE::CHAR);
        var_tab.set_type(pl3.value(), TYPE::INT);

        var_tab.set_value(pl2.value(), true);
        var_tab.set_value(pl3.value(), true);

        fout.open("table.txt");
        fout << var_tab;
        fout.close();

    } catch(const std::runtime_error& err) {
        constexpr size_t args_no_received = 2;
        std::cerr << err.what() << std::endl;
        std::cerr <<    _prs    << std::endl;
        std::exit(args_no_received);
    }
    return 0;
}
