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
                    bool success = false;
                    for (int i = 0; i < TokenLibrary::nTokens; i++)
                    {
                        Token::TokenizeResult result = TokenLibrary::tokenConstructors[i].tokenize(fileData, currPos);
                        if (result.success) {
                            tokens->push_back(result.token);
                            currPos = result.newPos;
                            success = true;
                            i = TokenLibrary::nTokens;
                        }
                    }
                    if (!success) {
                        throw new TokenizationException("Failed to tokenize file at character "+currPos);
                    }
                    break;
            }
        }
    }
    void Tokenizer::readFileData(std::string fileName) {
        std::ifstream file;
        file.exceptions(std::ios_base::failbit);
        file.open(fileName); // TODO: Catch any errors thrown?

        file.seekg(file.end);
        int fileLength = file.tellg();
        char* fileData = new char[fileLength];
        
        file.seekg(file.beg);
        file.read(fileData, fileLength);
        file.close();


        this->fileLength = -1;
        this->fileData = new BasicArray::BasicCharArray(fileData, fileLength);
        this->fileLength = fileLength;
    }
}

namespace {
}
