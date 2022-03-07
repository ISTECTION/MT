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
    argparse::ArgumentParser _prs("MT", "0.0.1");
    _prs.add_argument("-i", "--input")
        .help( "path to input files" )
        .required();

    _prs.add_argument("-t", "--table")
        .help("Print table to console")
        .default_value(false)
        .implicit_value(true);

    try {
        using ::std::filesystem::path;
        _prs.parse_args(argc, argv);

        path _inp = _prs.get<std::string>("-i");
        translator trs(_inp, _prs);

    } catch(const std::runtime_error& err) {
        constexpr size_t args_no_received = 2;
        std::cerr << err.what() << std::endl;
        std::cerr <<    _prs    << std::endl;
        std::exit(args_no_received);
    }
    return 0;
}