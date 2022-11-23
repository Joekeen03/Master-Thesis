#include "EnumRegistry.h"

namespace EnumRegistry {
    const RegistryItem EnumRegistry::registerItem(std::string str) {
        if (finalized) {
            throw new RegistryException("Registry is not accepting any more items.");
        }
        for (int i = 0; i < registry.size(); i++)
        {
            if (registry[i].str == str) {
                throw new RegistryException("Attempted to register a duplicate item: "+str);
            }
        }
        const RegistryItem newItem = RegistryItem(str, nextID++, *this);
        registry.push_back(newItem);
        return newItem;
    }

    const RegistryItem EnumRegistry::getItem(int ID) {
        int& index = ID; // Just to make it clear that the ID & index are the same.
        if (index < 0 || index >= size()) {
            throw RegistryException("Index " + std::to_string(index) + " is out of bounds for registry of size "
                                        +std::to_string(registry.size())+".");
        }
        return registry[index];
    }
    
    int EnumRegistry::nextRegistryID = 0;
}