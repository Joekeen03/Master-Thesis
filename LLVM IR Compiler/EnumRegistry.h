#ifndef LLVM_IR_COMPILER_ENUMREGISTRY_H
#define LLVM_IR_COMPILER_ENUMREGISTRY_H

#include <string>
#include <vector>

namespace EnumRegistry {
    class EnumRegistry;

    struct RegistryItem {
        friend EnumRegistry;
        private:
            RegistryItem(std::string strArg, int IDArg, EnumRegistry& registryArg) : str(strArg), ID(IDArg), registry(registryArg) {}
        public:
            const std::string str;
            const int ID;
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
            std::vector<RegistryItem> registry;
            bool finalized = false;
        public:
            const int registryID;
            const std::string registryName;

            EnumRegistry(std::string registryNameArg) : registryName(registryNameArg), registryID(nextRegistryID++) {}
            RegistryItem registerItem(std::string str);
            int finalize() { finalized = true; return size(); }
            int size() { return registry.size(); }
            const RegistryItem getItem(int index);

    };

    class RegistryException : public std::runtime_error {
        public:
            RegistryException(std::string msg) : runtime_error(msg.c_str()) {}
    };
}
#endif // LLVM_IR_COMPILER_ENUMREGISTRY_H