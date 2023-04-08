#include "LocalsMap.h"

namespace CodeGen {
    
    LocalsMapValue LocalsMap::getLocalEntry(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> identifier) const {
        if (Lib::isType<const Expressions::ExpressionLocalNamedIdentifier>(identifier)) {
            auto castIdentifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(identifier);
            return namedLocals->at(castIdentifier->name);
        } else if (Lib::isType<const Expressions::ExpressionLocalUnnamedIdentifier>(identifier)) {
            auto castIdentifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(identifier);
            return unnamedLocals->at(castIdentifier->ID);
        } else {
            throw std::runtime_error("LocalsMap::getLocalEntry(ExpressionLocalIdentifier) received an unsupported identifier: "+identifier->getName());
        }
    }
    
    LocalsMapValue LocalsMap::getLocalEntry(std::shared_ptr<const Expressions::ExpressionIdentifier> identifier) const {
        if (Lib::isDerivedType<const Expressions::ExpressionLocalIdentifier>(identifier)) {
            return getLocalEntry(std::dynamic_pointer_cast<const Expressions::ExpressionLocalIdentifier>(identifier));
        } else {
            throw std::runtime_error("LocalsMap::getLocalEntry(ExpressionIdentifier) received an unsupported identifier: "+identifier->getName());
        }
    }

    LocalsMapValue LocalsMap::getLocalEntry(const Expressions::ExpressionLocalUnnamedIdentifier identifier) const {
        return unnamedLocals->at(identifier.ID);
    } // getLocalEntry(ExpressionLocalUnnamedIdentifier)

    LocalsMapValue LocalsMap::getLocalEntry(const Expressions::ExpressionLocalNamedIdentifier identifier) const {
        return namedLocals->at(identifier.name);
    } // getLocalEntry(ExpressionLocalNamedIdentifier)

    //

    std::shared_ptr<LocalsMap> LocalsMap::createLocalsMap(std::string functionName, LocalsTypeListConstPtr paramLocals,
                                                        LocalsTypeListConstPtr instructionLocals, std::shared_ptr<const Types::Type> returnType) {
        auto namedLocals = std::make_shared<std::map<const std::string, const LocalsMapValue>>();
        auto unnamedLocals = std::make_shared<std::map<const int, const LocalsMapValue>>();
        int nextLocalPosition = 1; // First local is at one above the stack pointer.
        for (auto instructionLocalsIterator = instructionLocals->rbegin(); instructionLocalsIterator != instructionLocals->rend() ; instructionLocalsIterator++) {
            auto instructionLocal = *instructionLocalsIterator;
            if (Lib::isType<Expressions::ExpressionLocalNamedIdentifier>(instructionLocal.first)) {
                auto castLocal = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(instructionLocal.first);
                auto localType = instructionLocal.second;
                namedLocals->emplace(castLocal->name, LocalsMapValue(localType, nextLocalPosition));
                nextLocalPosition += localType->getSizeBytesCeil();
            } else if (Lib::isType<Expressions::ExpressionLocalUnnamedIdentifier>(instructionLocal.first)) {
                auto castLocal = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(instructionLocal.first);
                auto localType = instructionLocal.second;
                unnamedLocals->emplace(castLocal->ID, LocalsMapValue(localType, nextLocalPosition));
                nextLocalPosition += localType->getSizeBytesCeil();
            } else {
                throw std::runtime_error("LocalsMap::createLocalsMaps received an unsupported identifier in instructionLocals: "
                                            +instructionLocal.first->getName());
            }
        }
        for (auto paramLocalsIterator = paramLocals->rbegin(); paramLocalsIterator != paramLocals->rend() ; paramLocalsIterator++) {
            auto paramLocal = *paramLocalsIterator;
            if (Lib::isType<Expressions::ExpressionLocalNamedIdentifier>(paramLocal.first)) {
                auto castLocal = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(paramLocal.first);
                auto localType = paramLocal.second;
                namedLocals->emplace(castLocal->name, LocalsMapValue(localType, nextLocalPosition));
                nextLocalPosition += localType->getSizeBytesCeil();
            } else if (Lib::isType<Expressions::ExpressionLocalUnnamedIdentifier>(paramLocal.first)) {
                auto castLocal = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(paramLocal.first);
                auto localType = paramLocal.second;
                unnamedLocals->emplace(castLocal->ID, LocalsMapValue(localType, nextLocalPosition));
                nextLocalPosition += localType->getSizeBytesCeil();
            } else {
                throw std::runtime_error("LocalsMap::createLocalsMaps received an unsupported identifier in instructionLocals: "
                                            +paramLocal.first->getName());
            }
        }
        LocalsMapValue returnEntry = LocalsMapValue(returnType, nextLocalPosition);
        // This constructor is private, so we have to initialize it, then have std::make_shared use the public
        //  copy-constructor.
        auto localsMap = LocalsMap(functionName, namedLocals, unnamedLocals, returnEntry);
        return std::make_shared<LocalsMap>(localsMap);
    }
    // TODO: Add ability to query parameters' types and caller positions; i.e. information the calling function might need
    //  to call this function. Possibly not necessary.

}