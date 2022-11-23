#ifndef LLVM_IR_COMPILER_ENUMREGISTRY_H
#define LLVM_IR_COMPILER_ENUMREGISTRY_H

#include <string>
#include <vector>

namespace EnumRegistry {
    /* Class for creating an enum-like set of identifiers, each associated with an integer ID and a string,
            which are unique w/in the identifier's associated registry.
    -For each EnumRegistry registry, you can register identifier strings, which returns a RegistryItem -RegistryItems hold the string, the 
corresponding integer ID, and a reference to the master registry. -Can iterate over all the RegistryItems held by a registry.
    */
    class EnumRegistry;

    struct RegistryItem {
        friend EnumRegistry;
        private:
            RegistryItem(std::string strArg, int IDArg, EnumRegistry& registryArg) : str(strArg), ID(IDArg), registry(registryArg) {}
        public:
            const std::string str;
            const int ID; // == to the RegistryItem's index w/in its Registry's vector<RegistryItem>
            const EnumRegistry& registry; // FIXME: Is this safe?
            bool operator==(const RegistryItem &other) const {
                return (&registry == &other.registry) // Same registry
                        && (ID == other.ID); // Same ID
            }
    };

    class EnumRegistry {
        private:
            int nextID;
            static int nextRegistryID;
            std::vector<const RegistryItem> registry;
            bool finalized = false;
        public:
            const int registryID;
            const std::string registryName;

            EnumRegistry(std::string registryNameArg) : registryName(registryNameArg), registryID(nextRegistryID++) {}
            /* Register a string in the registry.
                If the string is unique (not already w/in th registry), returns a const RegistryItem containing the string,
                a unique ID (the RegistryItem's index w/in the internal vector<>), and a reference to the registry.
                Otherwise, throws a RegistryException.
            */
            const RegistryItem registerItem(std::string str);

            // Finalizes the registry. Prohibits any more string from being added to the registry, and returns size()
            int finalize() { finalized = true; return size(); }

            // Returns the number of strings in the registry
            int size() { return registry.size(); }

            /* Gets the RegistryItem with the specified ID (aka index w/in the internal vector<>).
                If the ID/index is within the registry's bounds (ID>=0 & ID <size()), it returns the corresponding RegistryItem.
                Otherwise (ID/index out of bounds) it throws a RegistryException.
            */
            const RegistryItem getItem(int ID);

    };

    class RegistryException : public std::runtime_error {
        public:
            RegistryException(std::string msg) : runtime_error(msg.c_str()) {}
    };
}
#endif // LLVM_IR_COMPILER_ENUMREGISTRY_H