#include "AttributeIDProcessor.h"

#include "Tokens/TokenKeyword.h"
#include "Tokens/TokenAttributeID.h"
#include "Tokens/TokenCurlyBrace.h"
#include "Tokens/TokenParenthesis.h"
#include "Tokens/TokenOperator.h"
#include "Tokens/TokenMetadataNodeStart.h"

namespace {
    enum BuildingAttributeState {
        ID,
        equals,
        leftCurly,
        rightCurly,
        notBuilding,
    };
}

namespace AttributeIDProcessor {
    ProcessedTokens AttributeIDProcessor::parseAttributeGroups(Tokenizer::tokensArrayPointer tokens) {
        Tokenizer::tokensArrayPointer processedTokens = std::make_shared<Tokenizer::tokensArray>();
        std::shared_ptr<std::vector<Tokenizer::tokensArray>> attributeGroups = std::make_shared<std::vector<Tokenizer::tokensArray>>();
        BuildingAttributeState buildState = notBuilding;
        int nestedParen = 0;
        int nestedCurly = 0;
        int id = -1;
        std::shared_ptr<const Token::TokenKeyword> startToken;
        for (auto&& tokenPointer : *tokens) {
            // std::cout << tokenPointer->getName() << " @ " << tokenPointer->srcPos << '\n';
            switch (buildState) {
                case notBuilding: {
                    if (typeid(*tokenPointer) == typeid(Token::TokenKeyword)
                        && (std::dynamic_pointer_cast<const Token::TokenKeyword>(tokenPointer)->registryItem == ReservedWords::attributes)) {
                            // Attribute groups are module-level, so they shouldn't be defined in any blocks, argument lists, etc.
                            if (nestedParen > 0) {
                                throw AttributeIDException("Attempted to define attribute group inside parentheses at source position "
                                                            +tokenPointer->srcPos);
                            } else if (nestedCurly > 0) {
                                throw AttributeIDException("Attempted to define attribute group inside curly braces at source position "
                                                            +tokenPointer->srcPos);
                            }
                            startToken = std::dynamic_pointer_cast<const Token::TokenKeyword>(tokenPointer);
                            buildState = ID;
                    } else {
                        processedTokens->push_back(tokenPointer);
                        if (typeid(*tokenPointer) == typeid(Token::TokenCurlyBrace)) {
                            nestedCurly += std::dynamic_pointer_cast<const Token::TokenCurlyBrace>(tokenPointer)->leftBrace ? 1 : -1;
                        } else if (typeid(*tokenPointer) == typeid(Token::TokenMetadataNodeStart)) {
                            nestedCurly += 1;
                        } else if (typeid(*tokenPointer) == typeid(Token::TokenParenthesis)) {
                            nestedParen += std::dynamic_pointer_cast<const Token::TokenParenthesis>(tokenPointer)->leftParen ? 1 : -1;
                        } 
                    }
                    break;
                }
                case ID: {
                    if (typeid(*tokenPointer) == typeid(Token::TokenAttributeID)) {
                        id = std::dynamic_pointer_cast<const Token::TokenAttributeID>(tokenPointer)->attributeID;
                        // Are any of these actual restrictions on attribute group definitions or attribute IDs?
                        if (id < 0) {
                            throw AttributeIDException("Attribute ID at source position "+std::to_string(tokenPointer->srcPos)+" was negative.");
                        } else if (id < attributeGroups->size()) {
                            throw AttributeIDException("Attempted to re-define attribute group for attribute ID "+std::to_string(id)
                                                        +" at source position "+std::to_string(tokenPointer->srcPos)+".");
                        } else if (id > attributeGroups->size()) {
                            throw AttributeIDException("Attribute group IDs must be defined consecutively - 1, 2, 3...; attribute group definition\
                                                        for attribute ID +"+std::to_string(id)+" at source position "+std::to_string(tokenPointer->srcPos)
                                                        +" was not the expected ID, "+std::to_string(attributeGroups->size()+1)+".");
                        } else {
                            // Better way to do this, which doesn't just implicitly depend on push_back(...) inserting at the specified ID?
                            attributeGroups->push_back(Tokenizer::tokensArray());
                            buildState = equals;
                        }
                    } else {
                        throw AttributeIDException("Expected TokenAttributeID at source position "+std::to_string(tokenPointer->srcPos)+", received "+tokenPointer->getName());
                    }
                    break;
                }
                case equals: {
                    if (typeid(*tokenPointer) == typeid(Token::TokenOperator)
                        && std::dynamic_pointer_cast<const Token::TokenOperator>(tokenPointer)->registryItem == Operators::equals) {
                            buildState = leftCurly;
                    } else {
                        throw AttributeIDException("Expected equals sign at source position "+std::to_string(tokenPointer->srcPos)+", received "+tokenPointer->getName());
                    }
                    break;
                }
                case leftCurly: {
                    if (typeid(*tokenPointer) == typeid(Token::TokenCurlyBrace)
                        && std::dynamic_pointer_cast<const Token::TokenCurlyBrace>(tokenPointer)->leftBrace) {
                            buildState = rightCurly;
                    } else {
                        throw AttributeIDException("Expected left curly brace at source position "+std::to_string(tokenPointer->srcPos)+", received "+tokenPointer->getName());
                    }
                    break;
                }
                case rightCurly: { // Just add the tokens and let the parser figure out if they're valid.
                    if (typeid(*tokenPointer) == typeid(Token::TokenCurlyBrace)
                        && !std::dynamic_pointer_cast<const Token::TokenCurlyBrace>(tokenPointer)->leftBrace) {
                            buildState = notBuilding;
                    } else if (typeid(*tokenPointer) == typeid(Token::TokenAttributeID)) {
                        // FIXME Can attribute groups include other attribute groups?
                        //  E.g. Something like 'attributes #4 = { noundef #2 #1 }
                        throw AttributeIDException("Nested attribute groups are forbidden. Attribute group defined starting at source position"
                                                    +std::to_string(startToken->srcPos)+" contained an attribute ID at source position"
                                                    +std::to_string(tokenPointer->srcPos)+".");
                    } else {
                        (*attributeGroups)[id].push_back(tokenPointer);
                    }
                    break;
                }
            }
        }
        std::vector<const Tokenizer::tokensArray> temp;
        // Can't seem to convert directly between shared_ptr<vector<tokensArray>> and shared_ptr<const vector<const tokensArray>>
        for (auto &&attrGroup : (*attributeGroups))
        {
            temp.push_back(attrGroup);
        }
        std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroupsFinal
            = std::make_shared<const std::vector<const Tokenizer::tokensArray>>(temp);
        return ProcessedTokens(processedTokens, attributeGroupsFinal);
    }

    SubstitutedTokens AttributeIDProcessor::substituteAttributeGroups(Tokenizer::tokensArrayPointer tokens) {
        ProcessedTokens procTokens = parseAttributeGroups(tokens);
        Tokenizer::tokensArrayPointer substitutedTokens = std::make_shared<Tokenizer::tokensArray>();
        std::shared_ptr<std::vector<int>> mappings = std::make_shared<std::vector<int>>();
        for (auto &&tokenPointer : *(procTokens.tokens)) {
            // std::cout << tokenPointer->getName() << '@' << tokenPointer->srcPos << '\n';
            if (typeid(*tokenPointer) == typeid(Token::TokenAttributeID)) {
                int id = std::dynamic_pointer_cast<const Token::TokenAttributeID>(tokenPointer)->attributeID;
                if (id < 0 || id >= procTokens.attributeGroups->size()){
                    throw AttributeIDException("Undefined attribute ID "+std::to_string(id)+" at "+std::to_string(tokenPointer->srcPos)+".");
                }
                for (auto &&attrGroupToken : (*procTokens.attributeGroups)[id]) {
                    substitutedTokens->push_back(attrGroupToken);
                    mappings->push_back(id);
                }
                
            } else {
                substitutedTokens->push_back(tokenPointer);
                mappings->push_back(-1);
            }
        }
        // for (auto &&tokenPointer : *substitutedTokens)
        // {
        //     std::cout << tokenPointer->getName() << '@' << tokenPointer->srcPos << '\n';
        // }
        
        return SubstitutedTokens(substitutedTokens, mappings, procTokens.attributeGroups);
    }
}