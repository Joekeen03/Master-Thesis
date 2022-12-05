#include <memory>

namespace Lib {
    template <typename T>
    inline std::shared_ptr<const T> makePointerToConst(std::shared_ptr<T> obj) { return std::dynamic_pointer_cast<const T>(obj); }
}