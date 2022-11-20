#include <string>
#include <iostream>

#include "Tokenizer.h"

int main(int argv, char* argc[]) {
    Tokenizer::Tokenizer tokenizer;
    std::string fileName = "Test/mainOnly.ll";
    try {
        tokenizer.Tokenize(fileName);
    } catch (const Tokenizer::TokenizationException& e) {
        std::cout << "Tokenization Exception:\n" << e.what() << '\n';
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch (const std::logic_error& e) {
        std::cout << e.what() << '\n';
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    
    return 0;
}