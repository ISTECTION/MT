#include "argparse/argparse.hpp"
#include "cxxtimer/cxxtimer.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <string>

#include "assembler.hpp"

int main(int argc, const char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    argparse::ArgumentParser _prs("MT", "1.0.0");
    _prs.add_argument("-i", "--input").help( "path to input files" ).required();

    _prs.add_argument("-t", "--table").help("Print table to console").default_value(false).implicit_value(true);
    _prs.add_argument("--parse-table").help("Print table to console").default_value(false).implicit_value(true);

    try {
        using ::std::filesystem::path;
        _prs.parse_args(argc, argv);
        path _inp = _prs.get<std::string>("-i");

        cxxtimer::Timer _timer(true);
        assembler _assembler(_inp);


        _timer.stop();
        std::cout << _timer;

        _assembler.print_table(_prs.get<bool>("--table"));
        if (_prs.get<bool>("--parse-table")) {
            _assembler.print_parse_table(_prs.get<bool>("--parse-table")); }

    } catch(const std::runtime_error& err) {
        constexpr std::size_t args_no_received { 2 };
        std::cerr << err.what() << std::endl;
        std::cerr <<    _prs    << std::endl;
        std::exit(args_no_received);
    }
    return 0;
}