#include "EnumRegistry.h"

namespace EnumRegistry {
    RegistryItem EnumRegistry::registerItem(std::string str) {
        if (finalized) {
            throw new RegistryException("Registry is not accepting any more items.");
        }
        for (int i = 0; i < registry.size(); i++)
        {
            if (registry[i].str == str) {
                throw new RegistryException("Attempted to register a duplicate item: "+str);
            }
        }
        RegistryItem newItem = RegistryItem(str, nextID++, *this);
        registry.push_back(newItem);
        return newItem;
    }

    const RegistryItem EnumRegistry::getItem(int index) {
        if (index < 0 || index >= size()) {
            throw RegistryException("Index " + std::to_string(index) + " is out of bounds for registry of size "
                                        +std::to_string(registry.size())+".");
        }
        return registry[index];
    }
    
    int EnumRegistry::nextRegistryID = 0;
}