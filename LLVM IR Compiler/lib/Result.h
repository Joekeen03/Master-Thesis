#ifndef LLVM_IR_COMPILER_RESULT_H
#define LLVM_IR_COMPILER_RESULT_H

namespace Lib {
    // A generic result which contains an operation's result, a new position, and whether the operation was actually successful.
    // If pointer=true, the result is a const shared_ptr to a <const T> object; otherwise, it is simply a <const T> object.
    // I.e.:
    //      pointer=true -> const shared_ptr<const T> result;
    //      pointer=false -> const T result;
    template <typename T, bool pointer>
    struct ResultConstMembers {
        using resultType = std::conditional_t<pointer, std::shared_ptr<const T>, T>;

        const resultType result;
        const int newPos;
        const bool success;
        ResultConstMembers() : result(), newPos(-1), success(false) {}
        ResultConstMembers(resultType resultArg, int newPosArg) : result(resultArg), newPos(newPosArg), success(true) {}
    };

    template <typename T>
    using ResultConstMembersPointer = ResultConstMembers<T, true>;

    template <typename T>
    using ResultConstMembersNoPointer = ResultConstMembers<T, false>;
    // A generic result which contains an operation's result, a new position, and whether the operation was actually successful.
    // If pointer=true, the result is a const shared_ptr to a <const T> object; otherwise, it is simply a <const T> object.
    // I.e.:
    //      pointer=true -> const shared_ptr<const T> result;
    //      pointer=false -> const T result;
    template <typename T, bool pointer>
    struct Result {
        using resultType = std::conditional_t<pointer, std::shared_ptr<const T>, T>;

        resultType result;
        int newPosition;
        bool success;
        Result() : result(), newPosition(-1), success(false) {}
        Result(resultType resultArg, int newPositionArg) : result(resultArg), newPosition(newPositionArg), success(true) {}
    };

    template <typename T>
    using ResultPointer = Result<T, true>;

    template <typename T>
    using ResultNoPointer = Result<T, false>;
}

#endif // LLVM_IR_COMPILER_RESULT_H