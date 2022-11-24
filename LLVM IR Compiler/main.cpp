#include <string>
#include <iostream>

#include "Tokenizer.h"
#include "Parser.h"
#include "EnumRegistry.h"

int main(int argv, char* argc[]) {
    Tokenizer::Tokenizer tokenizer;
    Parser::Parser parser;
    std::string fileName = "Test/mainOnly.ll";
    try {
        std::shared_ptr<Tokenizer::tokensArray> tokens = tokenizer.tokenize(fileName);
        std::cout << "Attempting to parse source_filename." << '\n';
        Parser::ParsingResult parseResult = parser.parseSourceFile(tokens, 0);
        Parser::ParsingResult parseResult2 = parser.parseDataLayout(tokens, parseResult.newPos);
        std::cout << std::boolalpha << "Success: " << parseResult2.success << '\n'
                  << std::resetiosflags(std::ios_base::boolalpha);
    } catch (const Tokenizer::TokenizationException& e) {
        std::cout << "Tokenization Exception:\n" << e.what() << '\n';
    } catch (const EnumRegistry::RegistryException& e) {
        std::cout << "Registry Exception:\n" << e.what() << '\n';
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch (const std::logic_error& e) {
        std::cout << e.what() << '\n';
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    
    return 0;
}