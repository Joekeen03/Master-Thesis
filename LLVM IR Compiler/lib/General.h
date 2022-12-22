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
}