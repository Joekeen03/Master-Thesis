#ifndef LLVM_IR_COMPILER_General_H
#define LLVM_IR_COMPILER_General_H

#include <memory>

namespace Lib {
    template <typename T>
    inline std::shared_ptr<const T> makePointerToConst(std::shared_ptr<T> obj) { return std::dynamic_pointer_cast<const T>(obj); }
    
            
    // FIXME Is it sufficiently clear that it expects a shared_ptr, and compares the dereferenced object's type to <T>?

    // Determines if the provided pointer points to an object of type <T>
    template<typename T, typename Y>
    inline bool isType(std::shared_ptr<Y> ptr) { return typeid(*ptr) == typeid(T); }

    // Determines if the provided pointer points to an instance of a subclass of class <T>
    // Specifically, it dynamically casts the pointer w/in the shared_ptr to const T*, and checks if that is not equal to
    //  nullptr. If the pointed-to-instance can be cast to type T successfully, dynamic_cast returns something other than
    //  nullptr, and this returns true; Otherwise (if the instance isn't an instance of a subclass of class <T>, or the
    //  pointer == nullptr), dynamic_cast returns nullptr, and this returns false.
    template<typename T, typename Y>
    inline bool isDerivedType(std::shared_ptr<Y> ptr) { return dynamic_cast<const T*>(ptr.get())!=nullptr; }

    // Wrapper for std::isBaseOf, for cases where you don't have access to 'val's type (or at least, I can't figure out
    //  how to get its type). For instance, when defining lambdas for use in std::visit, such as [](auto&& arg)...; I don't
    //  know how to find out arg's type, though the compiler clearly has to know it. Or if there might be a different way
    //  to write the lambda such that the type of arg is available.
    template <typename Base, typename Other>
    constexpr bool isBaseOf(Other val) {
        return std::is_base_of<Base, Other>();
    }
}

#endif // LLVM_IR_COMPILER_General_H