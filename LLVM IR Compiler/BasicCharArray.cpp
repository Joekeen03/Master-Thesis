#include "BasicCharArray.h"

namespace BasicArray {
    bool BasicCharArray::compareSubsectionToLiteral(int startPos, std::string literal) {
        // Subsection can't be equal to literal if array bounds are exceeded
        bool equal = (startPos+literal.size()) <= this->size;
        for (int i = 0; equal && (i < literal.size()); i++)
        {
            equal = literal[i] == (*this)[startPos+i];
        }
        return equal;
    }
}