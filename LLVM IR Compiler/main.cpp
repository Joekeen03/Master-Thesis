#include <string>
#include <iostream>
#include <fstream>

#include "Tokenizer.h"
#include "AttributeIDProcessor.h"
#include "Parser.h"
#include "EnumRegistry.h"
#include "Expressions/ExpressionFile.h"
#include "CodeGenerator.h"

constexpr auto llArgKey = "-ll";
constexpr auto asmArgKey = "-asm";
constexpr int filePathArgStartIndex = 1;

int main(int argc, char* argv[]) {
    if (argc != (filePathArgStartIndex+4)) {
        std::cout << "Usage: -ll <inputFilePath> -asm <outputFilePath>" << '\n';
        throw std::runtime_error("Incorrect number of arguments provided. Received "+std::to_string(argc)+" arguments.");
    }
    if (argv[filePathArgStartIndex+0] != std::string(llArgKey) || argv[filePathArgStartIndex+2] != std::string(asmArgKey)) {
        std::cout << "Usage: -ll <inputFilePath> -asm <outputFilePath>" << '\n';
        throw std::runtime_error("Incorrect argument keys provided. Received "+std::string(argv[filePathArgStartIndex+0]) + " and "+std::string(argv[filePathArgStartIndex+2])+".");
    }
    const auto inputFilePath = argv[filePathArgStartIndex+1];
    const auto outputFilePath = argv[filePathArgStartIndex+3];
    if (!std::filesystem::exists(inputFilePath)) {
        std::cout << "Input file does not exist." << '\n';
        throw std::runtime_error("Input file does not exist.");
    }
    if (std::filesystem::exists(outputFilePath)) {
        std::cout << "Warning: output file already exists." << '\n';
    }
    Tokenizer::Tokenizer tokenizer;
    AttributeIDProcessor::AttributeIDProcessor attributeProcessor;
    std::string outputFileName = "TestOutput/mainOnly.asm";
    try {
        std::shared_ptr<Tokenizer::tokensArray> tokens = tokenizer.tokenize(inputFilePath);
        AttributeIDProcessor::SubstitutedTokens substitutedTokens = attributeProcessor.substituteAttributeGroups(tokens);
        Parser::Parser parser(substitutedTokens);
        std::cout << "Attempting to parse source_filename." << '\n';
        auto parseResult = parser.parseSourceFile(0);
        auto parseResult2 = parser.parseDataLayout(parseResult.newPos);
        std::cout << std::boolalpha << "Success: " << parseResult2.success << '\n'
                  << std::resetiosflags(std::ios_base::boolalpha);
        std::shared_ptr<const Expressions::ExpressionFile> fileExpression = parser.parse();
        auto codeGen = CodeGen::CodeGenerator(fileExpression);
        auto generatedCode = codeGen.generateCode();
        std::ofstream outputFile;
        outputFile.open(outputFilePath, std::ios_base::trunc);
        for (auto &&line : *generatedCode) {
            outputFile << line << '\n';
        }
        outputFile.close();
        
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