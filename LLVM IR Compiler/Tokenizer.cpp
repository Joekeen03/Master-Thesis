#include "Tokenizer.h"

#include <typeinfo>

#include "Tokens/TokenComment.h"

namespace Tokenizer {
    std::shared_ptr<tokensArray> Tokenizer::tokenize(std::string fileName) {
        readFileData(fileName);
        std::shared_ptr<tokensArray> tokens = std::make_shared<std::vector<std::shared_ptr<const Token::Token>>>();
        int currPos = 0;
        while (fileData->positionInBounds(currPos)) {
            switch ((*fileData)[currPos]) {
                // FIXME Should I be tokenizing whitespace? Is LLVM IR at all whitespace sensitive? Kinda...it can have lists of
                //  keywords (such as when defining functions - "define dso_local i32"). Is this an issue?
                case ' ': // Skip whitespace
                case '\n':
                case '\t':
                    currPos++;
                    break;
                default:
                    // std::cout << "Attempting to tokenize at position "<< currPos << ". Character is '"<< (*fileData)[currPos] << "'" << '\n';
                    bool success = false;
                    for (int i = 0; i < TokenLibrary::nTokens; i++)
                    {
                        // std::cout << "Trying token type #"<< i << '\n';
                        
                        Token::TokenizeResult result = TokenLibrary::tokenConstructors[i]->tokenize(fileData, currPos);
                        if (result.success) {
                            std::cout << "Token created: " << result.token->getName() << '\n';
                            if (typeid(*result.token) != typeid(Token::TokenComment)) {
                                tokens->push_back(result.token);
                            } else {
                                std::cout << "Didn't add comment token to tokens array." << '\n';
                            }
                            currPos = result.newPos;
                            success = true;
                            i = TokenLibrary::nTokens;
                        }
                    }
                    if (!success) {
                        std::string charsAtPosition = "";
                        for (int i = 0; i < 10 && fileData->positionInBounds(currPos+i); i++)
                        {
                            charsAtPosition += (*fileData)[currPos+i];
                        }
                        throw TokenizationException(((std::string)"Failed to tokenize file at character "+std::to_string(currPos)\
                                                    +". Characters at position: "+charsAtPosition).c_str());
                    }
                    break;
            }
        }
        std::cout << "Tokenization successful."<<'\n';
        return tokens;
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
            
            // FIXME Encountered some weird bug after a merge commit, where reading would crash unless I told it to read fileLength-20 characters, for some
            //  reason. Regenerating the mainOnly.ll file (deleting it and calling 'make prepareTests') seemed to fix it. Some kind of weird corruption?
            file.seekg(file.beg);
            file.read(fileData, fileLength);
            file.close();
            this->fileLength = -1;
            this->fileData = new BasicArray::BasicCharArray(fileData, fileLength);
            this->fileLength = fileLength;

            std::cout << "File \""<< fileName << "\" read. It was " << fileLength << " characters long." << '\n';
        } catch (std::runtime_error e) {
            std::cout << "Error reading the input file." << '\n';
            throw e;
        }
    }
}

namespace {
}
