

// SPDX-License-Identifier: MIT

#pragma once

#include "TypeList.h"


namespace QkT {

    template<typename T>
    struct Function;

    // Base Function.
    template<typename Return, typename ...T>
    struct Function<Return(T...)> {
        using ReturnT = Return;
        using ArgsT   = TypeList<T...>;
    };

    template<typename Return, typename ...T>
    struct Function<Return(T...) noexcept> : Function<Return(T...)> {};

    // Pointers/references to function type.
    template<typename Return, typename ...T>
    struct Function<Return(*)(T...)> : Function<Return(T...)> {};

    template<typename Return, typename ...T>
    struct Function<Return(&)(T...)> : Function<Return(T...)> {};

    template<typename Return, typename ...T>
    struct Function<Return(&&)(T...)> : Function<Return(T...)> {};

    template<typename Return, typename ...T>
    struct Function<Return(*)(T...) noexcept> : Function<Return(T...)> {};

    template<typename Return, typename ...T>
    struct Function<Return(&)(T...) noexcept> : Function<Return(T...)> {};

    template<typename Return, typename ...T>
    struct Function<Return(&&)(T...) noexcept> : Function<Return(T...)> {};

    // Member function pointer helper: map to a function type with same qualifiers.
    #define MFP_TRAITS(...)                                                                        \
    template<typename C, typename R, typename... Args>                                             \
    struct Function<R(C::*)(Args...) __VA_ARGS__> : Function<R(Args...)> {};                       \
                                                                                                   \
    template<typename C, typename R, typename... Args>                                             \
    struct Function<R(C::*)(Args...) __VA_ARGS__ noexcept> : Function<R(Args...)> {}

    MFP_TRAITS();
    MFP_TRAITS(const);
    MFP_TRAITS(volatile);
    MFP_TRAITS(const volatile);
    MFP_TRAITS(&);
    MFP_TRAITS(const&);
    MFP_TRAITS(volatile&);
    MFP_TRAITS(const volatile&);
    MFP_TRAITS(&&);
    MFP_TRAITS(const&&);
    MFP_TRAITS(volatile&&);
    MFP_TRAITS(const volatile&&);

    #undef MFP_TRAITS

    // Functors/lambdas with a single, non-template operator().
    template<typename T>
    struct Function : Function<decltype(&std::remove_reference_t<T>::operator())> {};

} // namespace QkT
