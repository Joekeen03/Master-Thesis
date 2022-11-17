#include "Tokenizer.h"

namespace Tokenizer {
    std::vector<Token::Token> Tokenizer::Tokenize(std::string fileName) {
        readFileData(fileName);
        std::vector<Token::Token>* tokens = new std::vector<Token::Token>();
        int currPos = 0;
        while (currPos < fileLength) {
            switch ((*fileData)[currPos]) {
                case ' ': // Skip whitespace
                case '\n':
                    currPos++;
                    break;
                default:
                    std::cout << "Attempting to tokenize at position "<< currPos << ". Character is '"<< (*fileData)[currPos] << "'" << '\n';
                    bool success = false;
                    for (int i = 0; i < TokenLibrary::nTokens; i++)
                    {
                        std::cout << "Trying token type #"<< i << '\n';
                        
                        Token::TokenizeResult result = TokenLibrary::tokenConstructors[i]->tokenize(fileData, currPos);
                        if (result.success) {
                            tokens->push_back(result.token);
                            currPos = result.newPos;
                            success = true;
                            i = TokenLibrary::nTokens;
                        }
                    }
                    if (!success) {
                        throw TokenizationException("Failed to tokenize file at character "+currPos);
                    }
                    break;
            }
        }
    }

    void Tokenizer::readFileData(std::string fileName) {
        try {
            std::cout << "Attempting to read file: "<< fileName << '\n';
            std::ifstream file;
            file.exceptions(std::ios_base::failbit);
            file.open(fileName); // TODO: Catch any errors thrown?

            file.seekg(0, file.end);
            int fileLength = file.tellg();
            char* fileData = new char[fileLength];
            
            file.seekg(file.beg);
            file.read(fileData, fileLength);
            file.close();


            this->fileLength = -1;
            this->fileData = new BasicArray::BasicCharArray(fileData, fileLength);
            this->fileLength = fileLength;

            std::cout << "File \""<< fileName << "read. It was " << fileLength << " characters long." << '\n';
        } catch (std::runtime_error e) {
            std::cout << "Error reading the input file." << '\n';
            throw e;
        }
    }

    std::string Tokenizer::test() {
        return "asdf";
    }
}

namespace {
}
