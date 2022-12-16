
#include <fstream>
#include "../../lib/argparse.hpp"
#include "../../cyk/engine/grammar-parser.hpp"
#include "../engine/earley-parser.hpp"

int main(int argc, const char* argv[]) {
    argparse::ArgumentParser program("cyk");

    // Arguments: <grammar file> <word>

    program.add_argument("grammar")
            .help("Grammar file")
            .required();

    program.add_argument("word")
            .help("Word to check")
            .required();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    std::string grammar_file = program.get<std::string>("grammar");
    std::string word = program.get<std::string>("word");

    std::ifstream grammar_stream(grammar_file);

    if (!grammar_stream.is_open()) {
        std::cout << "Cannot open grammar file " << grammar_file << std::endl;
        exit(1);
    }

    Grammar grammar = GrammarParser().parse(grammar_stream);

    EarleyParser parser(grammar);

    std::cout << "Your grammar " << (parser.parse(word) ? "accepts" : "does not accept") << " word " << word << std::endl;

    return 0;
}