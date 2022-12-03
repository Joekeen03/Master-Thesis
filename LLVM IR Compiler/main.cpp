#include <string>
#include <iostream>

#include "Tokenizer.h"
#include "AttributeIDProcessor.h"
#include "Parser.h"
#include "EnumRegistry.h"
#include "Expressions/ExpressionFile.h"

int main(int argv, char* argc[]) {
    Tokenizer::Tokenizer tokenizer;
    AttributeIDProcessor::AttributeIDProcessor attributeProcessor;
    std::string fileName = "Test/mainOnly.ll";
    try {
        std::shared_ptr<Tokenizer::tokensArray> tokens = tokenizer.tokenize(fileName);
        AttributeIDProcessor::SubstitutedTokens substitutedTokens = attributeProcessor.substituteAttributeGroups(tokens);
        Parser::Parser parser(substitutedTokens);
        std::cout << "Attempting to parse source_filename." << '\n';
        auto parseResult = parser.parseSourceFile(0);
        auto parseResult2 = parser.parseDataLayout(parseResult.newPos);
        std::cout << std::boolalpha << "Success: " << parseResult2.success << '\n'
                  << std::resetiosflags(std::ios_base::boolalpha);
        std::shared_ptr<const Expressions::ExpressionFile> fileExpression = parser.parse();
    } catch (const Tokenizer::TokenizationException& e) {
        std::cout << "Tokenization Exception:\n" << e.what() << '\n';
    } catch (const Parser::ParsingException& e) {
        std::cout << "Parser Exception:\n" << e.what() << '\n';
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