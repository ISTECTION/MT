#include "argparse/argparse.hpp"

#include "const_table.hpp"
#include "var_table.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace Path_Const_Table {
    const std::filesystem::path
        keywords  = "file/keywords.txt",
        operation = "file/operation.txt";
}

int main(int argc, const char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    argparse::ArgumentParser _prs("MT", "1.0.0");
    _prs.add_argument("-i", "--input")
        .help( "path to input files" )
        .required();

    _prs.add_argument("-o", "--output")
        .help( "path to output files" );

    try {
        using ::std::filesystem::path;
        _prs.parse_args(argc, argv);

        std::optional _opt = _prs.present("-o");
        path _inp = _prs.get<std::string>("-i");
        path _out = _opt.has_value()
                  ? _prs.get<std::string>("-o")
                  : _inp.parent_path() / "output.txt";

        const_table<std::string>
            keywords_table (Path_Const_Table::keywords),
            operation_table(Path_Const_Table::operation);

        std::cout << operation_table.contains("==") << std::endl;


    } catch(const std::runtime_error& err) {
        constexpr size_t args_no_received = 2;
        std::cerr << err.what() << std::endl;
        std::cerr <<    _prs    << std::endl;
        std::exit(args_no_received);
    }

    return 0;
}
