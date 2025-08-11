

#pragma once

#include "View.h"

#include <type_traits>
#include <filesystem>


namespace Quirk::QkT {

    struct InvalidType {};

    template<typename T>
    struct IsInvalid {
        static constexpr bool Value = std::is_same_v<T, InvalidType>;
    };

    template<typename T>
    constexpr bool IsInvalid_V = IsInvalid<T>::Value;

    template<typename T>
    concept ValidType = !IsInvalid_V<T>;



    template<bool B>
    struct BoolConstant { static constexpr bool Value = B; };

    using TrueType  = BoolConstant<true>;
    using FalseType = BoolConstant<false>;



    template<typename T>
    struct IsNonVoid { static constexpr bool Value = std::negation_v<std::is_void<T>>; };

    template<typename T>
    constexpr bool IsNonVoid_V = IsNonVoid<T>::Value;



    template<typename T>
    struct AlwaysFalse : FalseType {};

    template<typename T>
    inline constexpr bool AlwaysFalse_V = AlwaysFalse<T>::Value;



    // removes cv qualifiers and pointer from the type
    template<typename T>
    using RemoveAllWrapperTypes_T = std::remove_cvref_t<std::remove_pointer_t<std::remove_cvref_t<T>>>;



    // Implementation detail, not for public use
    namespace Internal {
        template<typename T>
        struct IsViewImpl : FalseType {};

        template<typename T>
        struct IsViewImpl<View<T>> : TrueType {};
    }

    template<typename T>
    struct IsView : Internal::IsViewImpl<std::remove_cvref_t<T>> {};

    template<typename T>
    constexpr bool IsView_V = IsView<T>::Value;

    template<typename T>
    concept ViewType = IsView_V<T>;



    template<typename T>
    concept PathType = requires(T t) { { std::filesystem::path(t) }; };

    template<typename T>
    concept TupleType = requires { typename std::tuple_size<std::remove_cvref_t<T>>; };

    template<template<typename> typename Condition, typename T>
    concept ConditionTrait = requires { { Condition<T>::Value } -> std::convertible_to<bool>; };

} // namespace Quirk::QkT
